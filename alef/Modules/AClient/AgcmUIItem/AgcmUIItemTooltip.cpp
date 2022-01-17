#include "AgcmUIItem.h"
#include "AuStrTable.h"
#include "AgcmUILogin.h"

BOOL AgcmUIItem::StreamReadTooltipData(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgcmUIItem::StreamReadTooltipData() Error (OpenExcelFile) !!!\n");
		return FALSE;
	}

	INT32	lNumRow			= csExcelTxtLib.GetRow();
	INT32	lNumColumn		= csExcelTxtLib.GetColumn();

	// 멤버 변수에 대한 유효성 검사.
	if (lNumColumn > AGCMUIITEM_MAX_TOOLTIP)
		lNumColumn	= AGCMUIITEM_MAX_TOOLTIP;

	CHAR	*szColumnName	= NULL;

	for (int i = 0; i < lNumColumn; ++i)
	{
		szColumnName	= csExcelTxtLib.GetData(i, 1);
		if (!szColumnName || !szColumnName[0])
			continue;

		strcpy(m_aszTooptipTitle[i], szColumnName);
	}

	CHAR	*szRowName		= NULL;

	for (int i = 2; i < lNumRow; ++i)
	{
		szRowName		= csExcelTxtLib.GetData(1, i);
		if (!szRowName || !szRowName[0])
			continue;

		if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_WEAPON) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_WEAPON]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_ARMOUR) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_ARMOUR]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_SHIELD) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_SHIELD]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_RING) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_RING]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_AMULET) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_AMULET]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_POTION) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_POTION]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_SPIRITSTONE) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_SPIRITSTONE]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_TELEPORTSCROLL) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_TELEPORTSCROLL]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_ARROW) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_ARROW]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_BOLT) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_BOLT]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_TRANSFORMPOTION) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_TRANSFORMPOTION]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_CATALYST) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_CATALYST]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_RUNE) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_RUNE]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_RECEIPT) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_RECEIPT]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_SKILLBOOK) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_SKILLBOOK]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_SKILLSCROLL) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_SKILLSCROLL]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_ETC) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_ETC]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_RIDE) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_RIDE]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTIP_NAME_LANCER) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_LANCER]);
		else if (strcmp(szRowName, AGCDUIITEM_TOOLTOP_NAME_AVATAR) == 0)
			SetTooltipList(i, lNumColumn, &csExcelTxtLib, &m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_AVATAR]);
//		else
//		{
//			CHAR	*szTID		= csExcelTxtLib.GetData(0, i);
//			if (!szTID)
//				continue;
//
//			AgpdItemTemplate	*pcsItemTemplate	= m_pcsAgpmItem->GetItemTemplate(atoi(szTID));
//			if (!pcsItemTemplate)
//				continue;
//
//			AgcdUIItemTooltip	*pcsAttachData		= GetADItemTemplate(pcsItemTemplate);
//
//			SetTooltipList(i, lNumColumn, &csExcelTxtLib, pcsAttachData);
//		}
	}

	return TRUE;
}

BOOL AgcmUIItem::StreamReadTooltipDescData(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgcmUIItem::StreamReadTooltipData() Error (OpenExcelFile) !!!\n");
		return FALSE;
	}

	INT32	lNumRow			= csExcelTxtLib.GetRow();
	INT32	lNumColumn		= csExcelTxtLib.GetColumn();

	CHAR	*szTID			= NULL;
	CHAR	*szDescription	= NULL;

	for (int i = 1; i < lNumRow; ++i)
	{
		szTID			= csExcelTxtLib.GetData(0, i);
		szDescription	= csExcelTxtLib.GetData(2, i);

		if (szTID && szTID[0] && szDescription && szDescription[0])
		{
			AgpdItemTemplate	*pcsItemTemplate	= m_pcsAgpmItem->GetItemTemplate(atoi(szTID));
			if (pcsItemTemplate)
			{
				AgcdUIItemTooltipDesc	*pcsAttachTooltipData	= GetADItemTemplate(pcsItemTemplate);
				strncpy_s(pcsAttachTooltipData->m_szDescription, AGCMUIITEM_MAX_TOOLTIP_TEXT_LENGTH + 1, szDescription, _TRUNCATE );
			}
		}
	}
	
	return TRUE;
}

BOOL AgcmUIItem::SetTooltipList(INT32 lRow, INT32 lTotalColumn, AuExcelLib *pcsExcelTxtLib, AgcdUIItemTooltip *pcsItemTooltip)
{
	if (!pcsExcelTxtLib || !pcsItemTooltip)
		return FALSE;

	CHAR	*szBuffer	= NULL;

	pcsItemTooltip->m_nNumTooltip	= 0;

	for (int i = 2; i < lTotalColumn; ++i)
	{
		szBuffer	= pcsExcelTxtLib->GetData(i, lRow);
		if (!szBuffer)
			continue;

		pcsItemTooltip->m_alTooltipIndex[pcsItemTooltip->m_nNumTooltip++]	= i;
	}

	return TRUE;
}

BOOL AgcmUIItem::OpenToolTip(INT32 lIID, INT32 lX, INT32 lY, BOOL bShowPrice, BOOL bIsNPCTradeGrid)
{
	return OpenToolTip(m_pcsAgpmItem->GetItem(lIID), lX, lY, bShowPrice, bIsNPCTradeGrid);
}

BOOL AgcmUIItem::OpenToolTip(AgpdItem *pcsItem, INT32 lX, INT32 lY, BOOL bShowPrice, BOOL bIsNPCTradeGrid)
{
	// 필요한것 : 시작 좌표 , IID
	m_csItemToolTip.MoveWindow(lX, lY, m_csItemToolTip.w, m_csItemToolTip.h);

	m_csItemToolTip.DeleteAllStringInfo();
	
	SetItemToolTipInfo( pcsItem, bShowPrice, bIsNPCTradeGrid );

	m_csItemToolTip.ShowWindow( TRUE );

	return TRUE;
}

BOOL AgcmUIItem::CloseToolTip()
{
	m_csItemToolTip.ShowWindow( FALSE )		;
	m_csItemToolTip.DeleteAllStringInfo()	;	

	return TRUE;
}

/*****************************************************************
*   Function : CBOpenToolTip
*   Comment  : Item 관련 ToolTip을 연다 
*   Date&Time : 2003-11-10, 오후 12:24
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AgcmUIItem::CBOpenGridItemToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem* pThis = (AgcmUIItem*)pClass	;
		
	if ( !pcsSourceControl || pcsSourceControl->m_lType != AcUIBase::TYPE_GRID  || !pcsSourceControl->m_pcsBase )
		return FALSE;
	
	AcUIGrid*	pGrid = (AcUIGrid*)( pcsSourceControl->m_pcsBase );

	if (!pGrid->m_pToolTipAgpdGridItem || pGrid->m_pToolTipAgpdGridItem->m_eType != AGPDGRID_ITEM_TYPE_ITEM)
		return FALSE;

	BOOL	bShowPrice		= FALSE;
	BOOL	bIsNPCTradeGrid	= FALSE;

	if (pThis->m_bUIOpenNPCTrade)
	{
		/*
		if ((pThis->m_pcsNPCTradeGrid &&
			 pThis->m_pcsNPCTradeGrid == pGrid->m_pAgpdGrid) ||
			(pThis->m_pcsInventoryGrid &&
			 pThis->m_pcsInventoryGrid == pGrid->m_pAgpdGrid))
		{
			 bShowPrice = TRUE;
		}
		*/

		bShowPrice	= TRUE;

		if (pThis->m_pcsNPCTradeGrid &&
			pThis->m_pcsNPCTradeGrid == pGrid->m_pAgpdGrid)
		{
			bIsNPCTradeGrid	= TRUE;
		}
	}

	return pThis->OpenToolTip(pThis->m_pcsAgpmItem->GetItem(pGrid->m_pToolTipAgpdGridItem), pGrid->m_lItemToolTipX, pGrid->m_lItemToolTipY, bShowPrice, bIsNPCTradeGrid);
}

/*****************************************************************
*   Function : CBCloseToolTip
*   Comment  : Item 관련 ToolTip을 닫는다 
*   Date&Time : 2003-11-10, 오후 12:24
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AgcmUIItem::CBCloseGridItemToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem* pThis = (AgcmUIItem*)pClass		;

	pThis->m_csItemToolTip.ShowWindow( FALSE )		;
	pThis->m_csItemToolTip.DeleteAllStringInfo()	;	
	
	return TRUE;
}

void AgcmUIItem::SetItemToolTipInfo( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )	
{
	if( !pcsItem ) return ;

	AgpdItemTemplate* pTemplate = ( AgpdItemTemplate* )pcsItem->m_pcsItemTemplate;
	if( !pTemplate ) return;

	switch( pTemplate->m_nType )
	{
	case AGPMITEM_TYPE_EQUIP :
		{
			_SetItemToolTipEquip( pcsItem, bShowPrice, bIsNPCTradeGrid );
		}
		break;

	case AGPMITEM_TYPE_USABLE :
		{
			_SetItemToolTipUsable( pcsItem, bShowPrice, bIsNPCTradeGrid );
		}
		break;

	default :
		{
			AgcdUIItemTooltip* pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_ETC ];
			SetItemToolTipDetail( pcsItem, bShowPrice, bIsNPCTradeGrid, pToolTip );
		}
		break;
	}

	return;
}

void AgcmUIItem::_SetItemToolTipEquip( AgpdItem* pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	AgpdItemTemplateEquip* pTemplate = ( AgpdItemTemplateEquip* )pcsItem->m_pcsItemTemplate;
	if( !pTemplate ) return;

	AgcdUIItemTooltip* pToolTip = NULL;
	INT32 nItemConvertType = -1;

	// 아이템 종류에 따라 사용해야할 툴팁의 종류를 결정한다.
	switch( pTemplate->m_nKind )
	{
	case AGPMITEM_EQUIP_KIND_WEAPON :
		{
			AgpdItemTemplateEquipWeapon* pTemplateWeapon = ( AgpdItemTemplateEquipWeapon* )pTemplate;
			if( pTemplateWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE )
			{
				pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_LANCER ];
			}
			else
			{
				pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_WEAPON ];
				nItemConvertType = AGPDITEMCONVERT_MAX_WEAPON_SOCKET;
			}
		}
		break;

	case AGPMITEM_EQUIP_KIND_ARMOUR :
		{
			pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_ARMOUR ];

			if( m_pcsAgpmItemConvert->IsArmour( pcsItem ) )
			{
				nItemConvertType = AGPDITEMCONVERT_MAX_ARMOUR_SOCKET;
			}
		}
		break;

	case AGPMITEM_EQUIP_KIND_SHIELD :
		{
			pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_SHIELD ];
			nItemConvertType = AGPDITEMCONVERT_MAX_ETC_SOCKET;
		}
		break;

	case AGPMITEM_EQUIP_KIND_RING :
		{
			pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_RING ];
			nItemConvertType = AGPDITEMCONVERT_MAX_ETC_SOCKET;
		}
		break;

	case AGPMITEM_EQUIP_KIND_NECKLACE :
		{
			pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_AMULET ];
			nItemConvertType = AGPDITEMCONVERT_MAX_ETC_SOCKET;
		}
		break;

	case AGPMITEM_EQUIP_KIND_RIDE :
		{
			pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_RIDE ];
			nItemConvertType = AGPDITEMCONVERT_MAX_ETC_SOCKET;
		}
		break;

	default :
		{
			return;
		}
		break;
	}

	// 결정된 내용에 따라 툴팁의 세부항목들을 작성한다.
	SetItemToolTipDetail( pcsItem, bShowPrice, bIsNPCTradeGrid, pToolTip );

	// NPC와의 거래창이 아니고 개조가능한 아이템이라면 개조상태 아이콘을 추가한다.
	if( !bIsNPCTradeGrid && nItemConvertType >= 0 )
	{
		SetItemConvertIcon( pcsItem, nItemConvertType );
	}
}

void AgcmUIItem::_SetItemToolTipUsable( AgpdItem* pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	AgpdItemTemplateUsable* pTemplate = ( AgpdItemTemplateUsable* )pcsItem->m_pcsItemTemplate;
	if( !pTemplate ) return;

	AgcdUIItemTooltip* pToolTip = NULL;

	switch( pTemplate->m_nUsableItemType )
	{
	case AGPMITEM_USABLE_TYPE_POTION :			pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_POTION ];			break;
	case AGPMITEM_USABLE_TYPE_SPIRIT_STONE :	pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_SPIRITSTONE ];	break;
	case AGPMITEM_USABLE_TYPE_TELEPORT_SCROLL :	pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_TELEPORTSCROLL ];	break;
	case AGPMITEM_USABLE_TYPE_ARROW :			pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_ARROW ];			break;
	case AGPMITEM_USABLE_TYPE_BOLT :			pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_BOLT ];			break;
	case AGPMITEM_USABLE_TYPE_TRANSFORM :		pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_TRANSFORMPOTION ];break;
	case AGPMITEM_USABLE_TYPE_CONVERT_CATALYST :pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_CATALYST ];		break;
	case AGPMITEM_USABLE_TYPE_RUNE :			pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_RUNE ];			break;
	case AGPMITEM_USABLE_TYPE_RECEIPE :			pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_RECEIPT ];		break;
	case AGPMITEM_USABLE_TYPE_SKILL_BOOK :
		{
			EnumCallback( AGCMUIITEM_CB_SET_TOOLTIP, pcsItem, &m_csItemToolTip );
			return;
		}
		break;
	case AGPMITEM_USABLE_TYPE_SKILL_SCROLL :	pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_SKILLSCROLL ];	break;
	case AGPMITEM_USABLE_TYPE_AVATAR :			pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_AVATAR ];			break;
	default	:									pToolTip = &m_csItemTooltip[ AGCDUIITEM_TOOLTIP_INDEX_ETC ];			break;
	}

	SetItemToolTipDetail( pcsItem, bShowPrice, bIsNPCTradeGrid, pToolTip );
}

BOOL AgcmUIItem::_SetItemToolTipDetailName( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	SetItemToolTipName( pcsItem, bIsNPCTradeGrid );
	return TRUE;
}

BOOL AgcmUIItem::_SetItemToolTipDetailPrice( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	SetItemToolTipPrice(pcsItem, bShowPrice, bIsNPCTradeGrid);
	return TRUE;
}

BOOL AgcmUIItem::_SetItemToolTipDetailType( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	char strTemp[ 256 ] = { 0, };

	if( m_pcsAgpmAuctionCategory )
	{
		AgpdItemTemplate* pTemplate = ( AgpdItemTemplate* )pcsItem->m_pcsItemTemplate;
		if( !pTemplate ) return FALSE;

		AgpdAuctionCategory1Info** ppstFirstCategory = ( AgpdAuctionCategory1Info** )m_pcsAgpmAuctionCategory->GetCategory1()->GetObject( pTemplate->m_lFirstCategory );
		AgpdAuctionCategory2Info** ppstSecondCategory = ( AgpdAuctionCategory2Info** )m_pcsAgpmAuctionCategory->GetCategory2()->GetObject( pTemplate->m_lSecondCategory );

		if( ppstFirstCategory && *ppstFirstCategory && ppstSecondCategory && *ppstSecondCategory )
		{
			sprintf( strTemp, "%s : %s(%s)", ClientStr().GetStr( STI_TYPE ),
				( *ppstFirstCategory )->m_strName, ( *ppstSecondCategory )->m_strName );

			m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
			m_csItemToolTip.AddNewLine(14);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIItem::_SetItemToolTipDetailRank( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	char strTemp[ 256 ] = { 0, };

	AgpdItemTemplate* pTemplate = ( AgpdItemTemplate* )pcsItem->m_pcsItemTemplate;
	if( !pTemplate ) return FALSE;

	INT32 lTemplateItemRank	= m_pcsAgpmItemConvert->GetPhysicalItemRank( pTemplate );
	if( lTemplateItemRank > 0 )
	{
		INT32 lItemRank = m_pcsAgpmItemConvert->GetPhysicalItemRank( pcsItem );
		sprintf( strTemp, "%s : %s%s", ClientStr().GetStr( STI_ITEM_RANK ), 
			m_pcsAgpmItemConvert->GetPhysicalItemRankName( pcsItem ), ClientStr().GetStr( STI_STATICTEXT_RANK ) );

		if( lItemRank == lTemplateItemRank )
		{
			m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
		}
		else if( lItemRank > lTemplateItemRank )
		{
			m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_SPIRIT) );
		}
		else
		{
			m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY) );
		}

		if( m_pcsAgpmItemConvert->IsPhysicalConvertable( pcsItem ) != AGPDITEMCONVERT_RESULT_INVALID_ITEM &&
			m_pcsAgpmItemConvert->IsPhysicalConvertable( pcsItem ) != AGPDITEMCONVERT_RESULT_IS_EGO_ITEM &&
			m_pcsAgpmItemConvert->IsPhysicalConvertable( pcsItem ) != AGPDITEMCONVERT_RESULT_FAILED)
		{
			m_csItemToolTip.AddNewLine(14);

			ZeroMemory(strTemp, sizeof(strTemp));

			AgpdItemConvertADItem	*pcsAttachData	= m_pcsAgpmItemConvert->GetADItem(pcsItem);

			sprintf(strTemp, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_CONVERTABLE_PHYSICAL_RANK), 
							m_pcsAgpmItemConvert->GetPhysicalItemRank(pcsItem));

			m_csItemToolTip.AddString(strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));
		}

		if (m_pcsAgpmItemConvert->IsSpiritStoneConvertable(pcsItem) == AGPDITEMCONVERT_SPIRITSTONE_RESULT_IS_ALREADY_FULL ||
			m_pcsAgpmItemConvert->IsSpiritStoneConvertable(pcsItem) == AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS)
		{
			m_csItemToolTip.AddNewLine(14);

			ZeroMemory(strTemp, sizeof(strTemp));

			INT32	lSpiritStoneRank	= 1;

			for (lSpiritStoneRank = 1; lSpiritStoneRank <= AGPDITEMCONVERT_MAX_SPIRIT_STONE; ++lSpiritStoneRank)
				if (!m_pcsAgpmItemConvert->m_astTablePhysical[m_pcsAgpmItemConvert->GetPhysicalItemRank(pcsItem)].bIsConvertableSpirit[lSpiritStoneRank])
					break;

			sprintf(strTemp, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_CONVERTABLE_SS_RANK), 
							lSpiritStoneRank - 1);

			m_csItemToolTip.AddString(strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));
		}

		m_csItemToolTip.AddNewLine(14);
		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIItem::_SetItemToolTipDetailRequireLevel( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	char strTemp[ 256 ] = { 0, };
	INT32 lLevel = 0;
	m_pcsAgpmFactors->GetValue( &pcsItem->m_csRestrictFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL );

	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();

	if (lLevel > 0 && pcsSelfCharacter)
	{
		INT32	lCharLevel	= 0;
		m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lCharLevel, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL );

		INT32 lColor = GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL);

		if(pcsItem->m_pcsItemTemplate->m_lLimitedLevel == 0)
		{
			sprintf( strTemp, "%s : %3d%s", ClientStr().GetStr(STI_RESTRICT_LEVEL), lLevel,
				ClientStr().GetStr(STI_LEVEL) );
			if(lCharLevel < lLevel)
				lColor = GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY);
		}
		else
		{
			sprintf( strTemp, "%s : %2d ~ %2d%s", ClientStr().GetStr(STI_ITEM_LIMITED_LEVEL), lLevel,
				pcsItem->m_pcsItemTemplate->m_lLimitedLevel, ClientStr().GetStr(STI_LEVEL) );

			if(lCharLevel < lLevel || lCharLevel > pcsItem->m_pcsItemTemplate->m_lLimitedLevel)
				lColor = GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY);
		}

		m_csItemToolTip.AddString( strTemp, 12, lColor );
		return TRUE;
	}

	m_csItemToolTip.AddNewLine( 14 );
	return FALSE;
}

BOOL AgcmUIItem::_SetItemToolTipDetailRequireRace( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	char strString[ 256 ] = { 0, };

	INT32	lRestrictRace		= 0;
	INT32	lRestrictClass		= 0;

	m_pcsAgpmFactors->GetValue( &pcsItem->m_csRestrictFactor, &lRestrictRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE );

	BOOL bEquipConditionRace = _SetItemToolTipDetailEnableEquipRace( lRestrictRace );
	sprintf( strString, "%s :", ClientStr().GetStr( STI_RESTRICT_RACE ) );

	bool isFirst = true;
	if( lRestrictRace == AURACE_TYPE_NONE )
	{
		// 모든 종족 착용가능
		sprintf( strString + strlen( strString ), " %s", m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_ITEM_RESTRICT_RACEALL ) );
		bEquipConditionRace = TRUE;
	}
	else
	{
		// 휴먼 착용가능
		if( _IsEnableRace( lRestrictRace, AURACE_TYPE_HUMAN ) )
		{
			sprintf( strString + strlen( strString ), " %s", m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_ITEM_RESTRICT_RACEHUMAN ) );
			isFirst = false;
		}
		
		// 오크 착용가능
		if( _IsEnableRace( lRestrictRace, AURACE_TYPE_ORC ) )
		{
			CHAR* pFormat = isFirst ? " %s" : " / %s";
			sprintf( strString + strlen( strString ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_ITEM_RESTRICT_RACEORC ) );
			isFirst = false;
		}
	
		// 문엘프 착용가능
		if( _IsEnableRace( lRestrictRace, AURACE_TYPE_MOONELF ) )
		{
			CHAR* pFormat = isFirst ? " %s" : " / %s";
			sprintf( strString + strlen( strString ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_ITEM_RESTRICT_RACEMOONELF ) );
			isFirst = false;
		}

		// 드래곤시온 착용가능
		if( _IsEnableRace( lRestrictRace, AURACE_TYPE_DRAGONSCION ) )
		{
			CHAR* pFormat = isFirst ? " %s" : " / %s";
			sprintf( strString + strlen( strString ), " %s", m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_ITEM_RESTRICT_DRAGONSCION ) );
			isFirst = false;
		}
	}

	// 착용가능 여부에 따라 폰트 컬러 조정
	DWORD dwFontColor = bEquipConditionRace ? GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) : GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY);
	m_csItemToolTip.AddString( strString, 12, dwFontColor );
	return TRUE;
}

BOOL AgcmUIItem::_SetItemToolTipDetailRequireClass( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	char strString[ 256 ] = { 0, };

	INT32 lRestrictRace	= 0;
	INT32 lRestrictClass = 0;

	m_pcsAgpmFactors->GetValue( &pcsItem->m_csRestrictFactor, &lRestrictRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE );
	m_pcsAgpmFactors->GetValue( &pcsItem->m_csRestrictFactor, &lRestrictClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS );

	BOOL bEquipConditionClass = _SetItemToolTipDetailEnableEquipClass( lRestrictClass );
	

	sprintf( strString, "%s :", ClientStr().GetStr( STI_RESTRICT_CLASS ) );

	bool isFirst = true;
	CHAR strClassNames[ 128 ] = { 0, };
	if( lRestrictClass == 0 )
	{
		// 모든 클래스 착용가능
		sprintf( strString + strlen( strString ), " %s", m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_ITEM_RESTRICT_CLASSALL ) );
		bEquipConditionClass = TRUE;
	}
	else
	{
		// 전사계열 착용가능
		if( _IsEnableClass( lRestrictClass, AUCHARCLASS_TYPE_KNIGHT ) )
		{
			if( _CollectClassNames( strClassNames, 128, AUCHARCLASS_TYPE_KNIGHT, lRestrictRace ) )
			{
				CHAR* pFormat = isFirst ? " %s" : " / %s";
				sprintf( strString + strlen( strString ), " %s", strClassNames );
				isFirst = false;
			}
		}

		// 궁수계열 착용가능
		if( _IsEnableClass( lRestrictClass, AUCHARCLASS_TYPE_RANGER ) )
		{
			if( _CollectClassNames( strClassNames, 128, AUCHARCLASS_TYPE_RANGER, lRestrictRace ) )
			{
				CHAR* pFormat = isFirst ? " %s" : " / %s";
				sprintf( strString + strlen( strString ), " %s", strClassNames );
				isFirst = false;
			}
		}

		// 법사계열 착용가능
		if( _IsEnableClass( lRestrictClass, AUCHARCLASS_TYPE_MAGE ) )
		{
			if( _CollectClassNames( strClassNames, 128, AUCHARCLASS_TYPE_MAGE, lRestrictRace ) )
			{
				CHAR* pFormat = isFirst ? " %s" : " / %s";
				sprintf( strString + strlen( strString ), " %s", strClassNames );
				isFirst = false;
			}
		}

		// 시온계열 착용가능
		if( _IsEnableClass( lRestrictClass, AUCHARCLASS_TYPE_SCION ) )
		{
			if( _CollectClassNames( strClassNames, 128, AUCHARCLASS_TYPE_SCION, lRestrictRace ) )
			{
				CHAR* pFormat = isFirst ? " %s" : " / %s";
				sprintf( strString + strlen( strString ), " %s", strClassNames );
				isFirst = false;
			}
		}
	}

	DWORD dwFontColor = bEquipConditionClass ? GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) : GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY);
	
	// 클래스는 적합하더라도 종족이 맞지 않으면 빨간색 글씨로 표기
	if( !_SetItemToolTipDetailEnableEquipRace( lRestrictRace ) )
	{
		dwFontColor = GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY);
	}

	m_csItemToolTip.AddString( strString, 12, dwFontColor );
	return TRUE;
}

BOOL AgcmUIItem::_SetItemToolTipDetailEnableEquipRace( INT32 nEquipRequireRace )
{
	AgpdCharacter* ppdSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter ) return FALSE;

	AuRaceType eMyRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdSelfCharacter->m_csFactor );
	return _IsEnableRace( nEquipRequireRace, eMyRaceType );
}

BOOL AgcmUIItem::_SetItemToolTipDetailEnableEquipClass( INT32 nEquipRequireClass )
{
	AgpdCharacter* ppdSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter ) return FALSE;

	AuCharClassType eMyClassType = ( AuCharClassType )m_pcsAgpmFactors->GetClass( &ppdSelfCharacter->m_csFactor );
	return _IsEnableClass( nEquipRequireClass, eMyClassType );
}

BOOL AgcmUIItem::_SetItemToolTipDetailWeaponDamage( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	char strTemp[ 256 ] = { 0, };
	INT32	lMinDamage	= 0;
	INT32	lMaxDamage	= 0;

	INT32	lTemplateMinDamage	= 0;
	INT32	lTemplateMaxDamage	= 0;

	m_pcsAgpmFactors->GetValue( &pcsItem->m_csFactor, &lMinDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );
	m_pcsAgpmFactors->GetValue( &pcsItem->m_csFactor, &lMaxDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );

	m_pcsAgpmFactors->GetValue( &((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lTemplateMinDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );
	m_pcsAgpmFactors->GetValue( &((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lTemplateMaxDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );

	if (lMaxDamage > 0)
	{
		sprintf( strTemp, "%s : %d ~ %d", ClientStr().GetStr(STI_DAMAGE), lMinDamage, lMaxDamage);

		if (lTemplateMinDamage < lMinDamage ||
			lTemplateMaxDamage < lMaxDamage)
			m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_SPIRIT) );
		else
			m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIItem::_SetItemToolTipDetailWeaponSpeed( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	char strTemp[ 256 ] = { 0, };

	INT32	lAttackSpeed = 0;
	INT32	lTemplateAttackSpeed = 0;

	m_pcsAgpmFactors->GetValue( &pcsItem->m_csFactor, &lAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED );
	m_pcsAgpmFactors->GetValue( &((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lTemplateAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED );

	if (lAttackSpeed < 40)
		sprintf( strTemp, "%s : %s", ClientStr().GetStr(STI_ATTACK_SPEED), m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_ATTACKSPEED_VERYSLOW));
	else if (lAttackSpeed < 50)
		sprintf( strTemp, "%s : %s", ClientStr().GetStr(STI_ATTACK_SPEED), m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_ATTACKSPEED_SLOW));
	else if (lAttackSpeed < 60)
		sprintf( strTemp, "%s : %s", ClientStr().GetStr(STI_ATTACK_SPEED), m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_ATTACKSPEED_NORMAL));
	else if (lAttackSpeed < 70)
		sprintf( strTemp, "%s : %s", ClientStr().GetStr(STI_ATTACK_SPEED), m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_ATTACKSPEED_FAST));
	else
		sprintf( strTemp, "%s : %s", ClientStr().GetStr(STI_ATTACK_SPEED), m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_ATTACKSPEED_VERYFAST));

	if (lTemplateAttackSpeed < lAttackSpeed)
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_SPIRIT) )		;
	else
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) )		;

	return TRUE;
}

BOOL AgcmUIItem::_SetItemToolTipDetailDurability( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	char strTemp[ 256 ] = { 0, };
	INT32	lMaxItemDurability		= 0;
	INT32	lCurrentItemDurability	= 0;

	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lCurrentItemDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lMaxItemDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);

	BOOL	bLackAbility	= FALSE;

	if ( (AGPMITEM_TYPE_EQUIP == pcsItem->m_pcsItemTemplate->m_nType) &&
		 (AGPMITEM_EQUIP_KIND_RIDE == ((AgpdItemTemplateEquip*)pcsItem->m_pcsItemTemplate)->m_nKind))
	{
		if (pcsItem->m_pcsItemTemplate->m_bFreeDuration)
		{
			// 무제한이 설정되어 있다면
			sprintf( strTemp, ClientStr().GetStr(STI_NOLIMIT));
		}
		else
		{
			// 무제한이 설정되어 있지 않다면
			INT32 lHours = lCurrentItemDurability / (60 * 60);
			INT32 lMinutes = lCurrentItemDurability % (60 * 60) / 60;

			sprintf( strTemp, "[%s] %d %s %d %s", ClientStr().GetStr(STI_REMAIN_TIME), lHours, ClientStr().GetStr(STI_HOUR) ,lMinutes, ClientStr().GetStr(STI_MINUTE));
		}
	}
	else
	{
		sprintf( strTemp, "%s : %d/%d", ClientStr().GetStr(STI_DURABILITY), lCurrentItemDurability, lMaxItemDurability);

		if (m_pcsAgpmItem->GetItemDurabilityPercent(pcsItem) < 10)
			bLackAbility	= TRUE;
	}

	if (bLackAbility)
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY) );
	else
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

	return TRUE;
}

BOOL  AgcmUIItem::_SetItemToolTipDetailHeroicDamage( char* pName, AgpdItem* ppdItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	if( !pName || strlen( pName ) <= 0 ) return FALSE;
	if( !ppdItem || !ppdItem->m_pcsItemTemplate ) return FALSE;

	AgpdItemTemplate* ppdItemTemplate = ( AgpdItemTemplate* )ppdItem->m_pcsItemTemplate;

	int nDamageMin = 0;
	int nDamageMax = 0;

	int nDamageBaseMin = 0;
	int nDamageBaseMax = 0;

	m_pcsAgpmFactors->GetValue( &ppdItem->m_csFactor, &nDamageMin, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC );
	m_pcsAgpmFactors->GetValue( &ppdItem->m_csFactor, &nDamageMax, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC );

	m_pcsAgpmFactors->GetValue( &ppdItemTemplate->m_csFactor, &nDamageBaseMin, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC );
	m_pcsAgpmFactors->GetValue( &ppdItemTemplate->m_csFactor, &nDamageBaseMax, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC );

	if( nDamageMax > 0 )
	{
		char strText[ 256 ] = { 0, };
		sprintf( strText, "%s : %d ~ %d", pName, nDamageMin, nDamageMax );
		m_csItemToolTip.AddString( strText, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
		return TRUE;
	}

	return FALSE;
}

BOOL  AgcmUIItem::_SetItemToolTipDetailHeroicDefense( char* pName, AgpdItem* ppdItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )
{
	if( !pName || strlen( pName ) <= 0 ) return FALSE;
	if( !ppdItem || !ppdItem->m_pcsItemTemplate ) return FALSE;

	AgpdItemTemplate* ppdItemTemplate = ( AgpdItemTemplate* )ppdItem->m_pcsItemTemplate;
	int nDefense = 0;
	int nDefenseBase = 0;

	m_pcsAgpmFactors->GetValue( &ppdItem->m_csFactor, &nDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC );
	m_pcsAgpmFactors->GetValue( &ppdItemTemplate->m_csFactor, &nDefenseBase, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC );

	if( nDefense > 0 )
	{
		char strText[ 256 ] = { 0, };
		sprintf( strText, "%s : %d", pName, nDefense );
		m_csItemToolTip.AddString( strText, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIItem::SetItemToolTipDetail(AgpdItem* pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid, AgcdUIItemTooltip* pcsItemTooltip )
{
	if (!pcsItem || !pcsItemTooltip)
		return FALSE;

	char strTemp[256];

	BOOL	bAddString	= FALSE;

	for (int i = 0; i < pcsItemTooltip->m_nNumTooltip; ++i)
	{
		bAddString	= FALSE;

		memset( strTemp, '\0', sizeof( strTemp ) );

		if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_NAME) == 0)
		{
			bAddString = _SetItemToolTipDetailName( pcsItem, bShowPrice, bIsNPCTradeGrid );
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_PRICE) == 0)
		{
			bAddString = _SetItemToolTipDetailPrice( pcsItem, bShowPrice, bIsNPCTradeGrid );
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_TYPE) == 0)
		{
			bAddString = _SetItemToolTipDetailType( pcsItem, bShowPrice, bIsNPCTradeGrid );
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_ITEM_RANK) == 0)
		{
			bAddString = _SetItemToolTipDetailRank( pcsItem, bShowPrice, bIsNPCTradeGrid );
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_SECOND_CATEGORY) == 0)
		{
			if (m_pcsAgpmAuctionCategory)
			{
				AgpdAuctionCategory2Info	**ppstSecondCategory	= (AgpdAuctionCategory2Info **)	m_pcsAgpmAuctionCategory->GetCategory2()->GetObject(((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lSecondCategory);

				if (ppstSecondCategory && *ppstSecondCategory)
				{
					sprintf(strTemp, "%s", (*ppstSecondCategory)->m_strName);

					m_csItemToolTip.AddString(strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));

					bAddString	= TRUE;
				}
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_ATTACK_RANGE) == 0)
		{
//			INT32	lAttackRange	= 0;
//			m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lAttackRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);
//
//			if (lAttackRange <= 200)
//				sprintf( strTemp, "%s : %s", AGCDUIITEM_TOOLTIP_NAME_ATTACK_RANGE, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_ATTACKRANGE_SHORT) );
//			else
//				sprintf( strTemp, "%s : %s", AGCDUIITEM_TOOLTIP_NAME_ATTACK_RANGE, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_ATTACKRANGE_LONG) );
//
//			m_csItemToolTip.AddString( strTemp, 12, AGCMUIITEM_TOOLTIP_COLOR_NORMAL )		;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_DAMAGE) == 0)
		{
			bAddString = _SetItemToolTipDetailWeaponDamage( pcsItem, bShowPrice, bIsNPCTradeGrid );
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_ATTACK_SPEED) == 0)
		{
			bAddString = _SetItemToolTipDetailWeaponSpeed( pcsItem, bShowPrice, bIsNPCTradeGrid );
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_DEFENSE) == 0)
		{
			INT32	lPhysicalDefense	= 0;
			INT32	lTemplatePhysicalDefense	= 0;

			m_pcsAgpmFactors->GetValue( &pcsItem->m_csFactor, &lPhysicalDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );
			m_pcsAgpmFactors->GetValue( &((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lTemplatePhysicalDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );

			if (lPhysicalDefense > 0)
			{
				sprintf( strTemp, "%s : %d", ClientStr().GetStr(STI_DEFENSE), lPhysicalDefense);

				if (lTemplatePhysicalDefense < lPhysicalDefense)
					m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_SPIRIT) );
				else
					m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_HEROIC_DAMAGE) == 0)
		{
			bAddString = _SetItemToolTipDetailHeroicDamage( m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], pcsItem, bShowPrice, bIsNPCTradeGrid );
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_HEROIC_DEFENSE) == 0)
		{
			bAddString = _SetItemToolTipDetailHeroicDefense( m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], pcsItem, bShowPrice, bIsNPCTradeGrid );
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_BLOCK) == 0)
		{
			INT32	lBlockRate	= 0;
			INT32	lTemplateBlockRate	= 0;
			m_pcsAgpmFactors->GetValue( &pcsItem->m_csFactor, &lBlockRate, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL_BLOCK );
			m_pcsAgpmFactors->GetValue( &((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lTemplateBlockRate, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL_BLOCK );

			if (lBlockRate > 0)
			{
				sprintf( strTemp, "%s : %d %%", ClientStr().GetStr(STI_BLOCK), lBlockRate );

				if (lTemplateBlockRate < lBlockRate)
					m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_SPIRIT) );
				else
					m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_DURABILITY) == 0)
		{
			bAddString = _SetItemToolTipDetailDurability( pcsItem, bShowPrice, bIsNPCTradeGrid );
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_RESTORE_HP) == 0)
		{
			INT32	lHP	= 0;
			AgpdItemTemplateUsable	*pcsItemTemplateUsable	= (AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate;

			m_pcsAgpmFactors->GetValue(&pcsItemTemplateUsable->m_csEffectFactor, &lHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

			if (lHP > 0)
			{
				if (((AgpdItemTemplateUsablePotion *) pcsItemTemplateUsable)->m_bIsPercentPotion)
				{
					sprintf( strTemp, "%s %d%% %s", ClientStr().GetStr(STI_LIFE), lHP, ClientStr().GetStr(STI_RECOVER));
				}
				else
				{
					sprintf( strTemp, "%s %d %s", ClientStr().GetStr(STI_LIFE), lHP, ClientStr().GetStr(STI_RECOVER));
				}

				m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_RESTORE_MP) == 0)
		{
			INT32	lMP	= 0;
			AgpdItemTemplateUsable	*pcsItemTemplateUsable	= (AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate;

			m_pcsAgpmFactors->GetValue(&pcsItemTemplateUsable->m_csEffectFactor, &lMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);

			if (lMP > 0)
			{
				if (((AgpdItemTemplateUsablePotion *) pcsItemTemplateUsable)->m_bIsPercentPotion)
				{
					sprintf( strTemp, "%s %d%% %s", ClientStr().GetStr(STI_MANA), lMP, ClientStr().GetStr(STI_RECOVER));
				}
				else
				{
					sprintf( strTemp, "%s %d %s", ClientStr().GetStr(STI_MANA), lMP, ClientStr().GetStr(STI_RECOVER));
				}

				m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_REDUCE_POISON) == 0)
		{
			// 해독이란 개념이 아직 없다.
			//
			//
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_PHYSICAL_AR) == 0)
		{
			INT32	lPhysicalAR	= 0;
			m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lPhysicalAR, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_AR);

			if (lPhysicalAR > 0)
			{
				sprintf( strTemp, "%s : %d", ClientStr().GetStr(STI_PHYSICAL_AR), lPhysicalAR );
				DWORD dwColor = GetToolTipColor( pcsItem, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_AR );
				m_csItemToolTip.AddString( strTemp, 12, dwColor );

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_MAGICAL_AR) == 0)
		{
			INT32	lMagicalAR	= 0;
			m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lMagicalAR, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MAR);

			if (lMagicalAR > 0)
			{
				sprintf( strTemp, "%s : %d", ClientStr().GetStr(STI_MAGICAL_AR), lMagicalAR );
				DWORD dwColor = GetToolTipColor( pcsItem, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MAR );
				m_csItemToolTip.AddString( strTemp, 12, dwColor );

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_PHYSICAL_DR) == 0)
		{
			INT32	lPhysicalDR	= 0;
			m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lPhysicalDR, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_DR);

			if (lPhysicalDR > 0)
			{
				sprintf( strTemp, "%s : %d", ClientStr().GetStr(STI_PHYSICAL_DR), lPhysicalDR );
				DWORD dwColor = GetToolTipColor( pcsItem, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_DR );
				m_csItemToolTip.AddString( strTemp, 12, dwColor );

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_DURATION) == 0)
		{
			INT32	lDuration	= ((AgpdItemTemplateUsableTransform *) pcsItem->m_pcsItemTemplate)->m_ulDuration;

			lDuration	/= 1000;

			INT32	lDurationMin	= lDuration / 60;
			INT32	lDurationSec	= lDuration % 60;

			sprintf(strTemp, "%s : ", ClientStr().GetStr(STI_DURATION));

			if (lDurationMin > 0)
				sprintf(strTemp + strlen(strTemp), "%d%s ", lDurationMin, ClientStr().GetStr(STI_MINUTE));

			if (lDurationSec > 0)
				sprintf(strTemp + strlen(strTemp), "%d%s", lDurationSec, ClientStr().GetStr(STI_SECOND));

			m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

			bAddString	= TRUE;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_RESTRICT_LEVEL) == 0)
		{
			bAddString = _SetItemToolTipDetailRequireLevel( pcsItem,  bShowPrice, bIsNPCTradeGrid );
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_RESTRICT_RACE) == 0)
		{
			bAddString = _SetItemToolTipDetailRequireRace( pcsItem,  bShowPrice, bIsNPCTradeGrid );
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_RESTRICT_CLASS) == 0)
		{
			bAddString = _SetItemToolTipDetailRequireClass( pcsItem,  bShowPrice, bIsNPCTradeGrid );
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_USE_LEVEL) == 0)
		{
			INT32	lLevel	= 0;
			m_pcsAgpmFactors->GetValue( &pcsItem->m_csRestrictFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL );

			AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();

			if (lLevel > 0 && pcsSelfCharacter)
			{
				INT32	lCharLevel	= 0;
				m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lCharLevel, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL );

				sprintf( strTemp, "%s : %3d ", ClientStr().GetStr(STI_USE_LEVEL), lLevel );

				if (lCharLevel < lLevel)
					m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY) /*+ 0x00010000*/ );
				else
					m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_USE_RACE) == 0)
		{
			BOOL	bEquipConditionRace		= TRUE;

			INT32	lRestrictRace		= 0;
			INT32	lRestrictClass		= 0;

			m_pcsAgpmFactors->GetValue( &pcsItem->m_csRestrictFactor, &lRestrictRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE );

			AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();

			if (lRestrictRace > 0 && pcsSelfCharacter && lRestrictRace != m_pcsAgpmFactors->GetRace(&pcsSelfCharacter->m_csFactor))
				bEquipConditionRace	= FALSE;

			switch (lRestrictRace) {
			case AURACE_TYPE_HUMAN:
				sprintf(strTemp, "%s : %s", ClientStr().GetStr(STI_USE_RACE), m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_RESTRICT_RACEHUMAN));
				break;

			case AURACE_TYPE_ORC:
				sprintf(strTemp, "%s : %s", ClientStr().GetStr(STI_USE_RACE), m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_RESTRICT_RACEORC));
				break;

			case AURACE_TYPE_MOONELF:
				sprintf(strTemp, "%s : %s", ClientStr().GetStr(STI_USE_RACE), m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_RESTRICT_RACEMOONELF));
				break;

			case AURACE_TYPE_DRAGONSCION:
				sprintf(strTemp, "%s : %s", ClientStr().GetStr(STI_USE_RACE), m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_RESTRICT_DRAGONSCION));
				break;

			case AURACE_TYPE_NONE:
				sprintf(strTemp, "%s : %s", ClientStr().GetStr(STI_USE_RACE), m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_RESTRICT_RACEALL));
				break;
			}

			if (!bEquipConditionRace)
				m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY) /*+ 0x00010000*/ );
			else
				m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

			bAddString	= TRUE;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_ITEM_CONVERT) == 0)
		{
			SetItemToolTipConvertAttrInfo(pcsItem);

			if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i + 1]], AGCDUIITEM_TOOLTIP_NAME_NEW_LINE) == 0 &&
				m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem) == 0)
				++i;

			continue;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_USE_RUNE_LEVEL) == 0)
		{
			AgpdItemConvertADItemTemplate	*pcsItemConvertAttachData	= m_pcsAgpmItemConvert->GetADItemTemplate((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate);

			sprintf(strTemp, "%s : %d", ClientStr().GetStr(STI_USE_RUNE_LEVEL), pcsItemConvertAttachData->m_lRuneRestrictLevel);

			m_csItemToolTip.AddString(strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));

			bAddString	= TRUE;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_MAX_HP) == 0)
		{
			INT32	lAddMaxHP	= 0;

			if (pcsItem->m_pcsItemTemplate &&
				((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
				((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM)
			{
				m_pcsAgpmFactors->GetValue(&((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_csEffectFactor, &lAddMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
			}

			if (lAddMaxHP > 0)
			{
				sprintf(strTemp, "%s : +%d", ClientStr().GetStr(STI_MAX_HP), lAddMaxHP);

				m_csItemToolTip.AddString(strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_MAX_MP) == 0)
		{
			INT32	lAddMaxMP	= 0;

			if (pcsItem->m_pcsItemTemplate &&
				((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
				((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM)
			{
				m_pcsAgpmFactors->GetValue(&((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_csEffectFactor, &lAddMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
			}

			if (lAddMaxMP > 0)
			{
				sprintf(strTemp, "%s : +%d", ClientStr().GetStr(STI_MAX_MP), lAddMaxMP);

				m_csItemToolTip.AddString(strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_ADD_DAMAGE) == 0)
		{
			INT32	lAddDamage	= 0;

			if (pcsItem->m_pcsItemTemplate &&
				((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
				((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM)
			{
				m_pcsAgpmFactors->GetValue(&((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_csEffectFactor, &lAddDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
			}

			if (lAddDamage > 0)
			{
				sprintf(strTemp, "%s : +%d", ClientStr().GetStr(STI_ADD_DAMAGE), lAddDamage);

				m_csItemToolTip.AddString(strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_ADD_DEFENSE) == 0)
		{
			INT32	lAddDefense	= 0;

			if (pcsItem->m_pcsItemTemplate &&
				((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
				((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM)
			{
				m_pcsAgpmFactors->GetValue(&((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_csEffectFactor, &lAddDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
			}

			if (lAddDefense > 0)
			{
				sprintf(strTemp, "%s : +%d", ClientStr().GetStr(STI_ADD_DEFENSE), lAddDefense);

				m_csItemToolTip.AddString(strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_REPAIR_COST) == 0)
		{
			if (FALSE == pcsItem->m_pcsItemTemplate->m_bFreeDuration)
			{
				INT64	llRepairCost	= m_pcsAgpmEventItemRepair->GetItemRepairPrice(pcsItem, m_pcsAgcmCharacter->GetSelfCharacter());

				sprintf(strTemp, "%s : %I64d %s", ClientStr().GetStr(STI_REPAIR_COST), llRepairCost, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_MONEY_NAME));

				m_csItemToolTip.AddString(strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_MOVE_SPEED) == 0)
		{
			sprintf(strTemp, "%s : %d %s", ClientStr().GetStr(STI_MOVE_SPEED), pcsItem->m_pcsItemTemplate->m_lRunBuff / 1000, ClientStr().GetStr(STI_METER));
			m_csItemToolTip.AddString(strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));

			bAddString	= TRUE;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_SPIRIT_STONE_INFO) == 0)
		{
			SetItemToolTipSpiritStoneInfo(pcsItem);

			bAddString	= TRUE;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_RING_INFO) == 0)
		{
			SetItemToolTipRingInfo(pcsItem, pcsItemTooltip);

			bAddString	= TRUE;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_AMULET_INFO) == 0)
		{
			SetItemToolTipAmuletInfo(pcsItem, pcsItemTooltip);

			bAddString	= TRUE;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_NEW_LINE) == 0)
		{
			// 줄만 띄면 되니 암것도 안한다.

			// 만약 이거 바로 전것도 줄바꿈이면 이번건 무시한다.

			if (i > 0 &&
				strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i - 1]], AGCDUIITEM_TOOLTIP_NAME_NEW_LINE) == 0)
				continue;

			bAddString	= TRUE;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_RUNE_USE_WEAPON) == 0)
		{
			AgpdItemConvertADItemTemplate	*pcsAttachTemplateData	= m_pcsAgpmItemConvert->GetADItemTemplate((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate);

			if (pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_WEAPON_INDEX])
			{
				m_csItemToolTip.AddString(ClientStr().GetStr(STI_RUNE_USE_WEAPON), 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));
				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_RUNE_USE_ARMOUR) == 0)
		{
			AgpdItemConvertADItemTemplate	*pcsAttachTemplateData	= m_pcsAgpmItemConvert->GetADItemTemplate((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate);

			if (pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_ARMOUR_INDEX])// &&
				//!pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_ACCESSORY_RING1] &&
				//!pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_ACCESSORY_NECKLACE])
			{
				m_csItemToolTip.AddString(ClientStr().GetStr(STI_RUNE_USE_ARMOUR), 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));

				CHAR	szUsePartDesc[256];
				ZeroMemory(szUsePartDesc, sizeof(szUsePartDesc));

				sprintf(szUsePartDesc, "(");

				if (pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_BODY] &&
					ClientStr().GetStr(STI_RUNE_USE_PART_ARMOUR))
				{
					sprintf(szUsePartDesc + strlen(szUsePartDesc), "%s, ", ClientStr().GetStr(STI_RUNE_USE_PART_ARMOUR));
				}
				if (pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_HEAD] &&
					ClientStr().GetStr(STI_RUNE_USE_PART_HELMET))
				{
					sprintf(szUsePartDesc + strlen(szUsePartDesc), "%s, ", ClientStr().GetStr(STI_RUNE_USE_PART_HELMET));
				}
				if (pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_LEGS] &&
					ClientStr().GetStr(STI_RUNE_USE_PART_GREAVE))
				{
					sprintf(szUsePartDesc + strlen(szUsePartDesc), "%s, ", ClientStr().GetStr(STI_RUNE_USE_PART_GREAVE));
				}
				if (pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_FOOT] &&
					ClientStr().GetStr(STI_RUNE_USE_PART_FOOT))
				{
					sprintf(szUsePartDesc + strlen(szUsePartDesc), "%s, ", ClientStr().GetStr(STI_RUNE_USE_PART_FOOT));
				}
				if (pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_HANDS] &&
					ClientStr().GetStr(STI_RUNE_USE_PART_GAUNTLET))
				{
					sprintf(szUsePartDesc + strlen(szUsePartDesc), "%s, ", ClientStr().GetStr(STI_RUNE_USE_PART_GAUNTLET));
				}

				if (strlen(szUsePartDesc) > 1)
				{
					sprintf(szUsePartDesc + strlen(szUsePartDesc) - 2, ")");

					m_csItemToolTip.AddNewLine(14);
					m_csItemToolTip.AddString(szUsePartDesc, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));
				}

				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_RUNE_USE_SHIELD) == 0)
		{
			AgpdItemConvertADItemTemplate	*pcsAttachTemplateData	= m_pcsAgpmItemConvert->GetADItemTemplate((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate);

			if (pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_SHIELD_INDEX])
			{
				m_csItemToolTip.AddString(ClientStr().GetStr(STI_RUNE_USE_SHIELD), 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));
				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_RUNE_USE_RING) == 0)
		{
			AgpdItemConvertADItemTemplate	*pcsAttachTemplateData	= m_pcsAgpmItemConvert->GetADItemTemplate((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate);

			if (pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_RING_INDEX])
			{
				m_csItemToolTip.AddString(ClientStr().GetStr(STI_RUNE_USE_RING), 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));
				bAddString	= TRUE;
			}
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_RUNE_USE_AMULET) == 0)
		{
			AgpdItemConvertADItemTemplate	*pcsAttachTemplateData	= m_pcsAgpmItemConvert->GetADItemTemplate((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate);

			if (pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_NECKLACE_INDEX])
			{
				m_csItemToolTip.AddString(ClientStr().GetStr(STI_RUNE_USE_AMULET), 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));
				bAddString	= TRUE;
			}
		}
#define _TOOLTIPSTR_TO_SYSSTR //일부 컬럼을 외국어로 컬럼명을 번역할 때 에로사항(?)을 해결하기 위함. 이제 itemtooltip.txt 번역 안해도 됨!!!
#ifdef _TOOLTIPSTR_TO_SYSSTR
//#define AGCDUIITEM_TOOLTIP_NAME_BOW_SERIES	"활 계열용"
//#define AGCDUIITEM_TOOLTIP_NAME_CROSSBOW_SERIES		"석궁 계열용"
//#define AGCDUIITEM_TOOLTIP_NAME_DRAG_BUILDUP_ITEM		"아이템위로 드래그해서 강화 가능"
//#define AGCDUIITEM_TOOLTIP_NAME_SMITH_BUILDUP_ITEM		"대장장이를 통해 강화 가능"
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_BOW_SERIES) == 0)
		{
			m_csItemToolTip.AddString(ClientStr().GetStr(STI_BOW_SERIES), 12, GetTooltipColor( AGCMUIITEM_TOOLTIP_COLOR_DESCRIPTION ));
			bAddString	= TRUE;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_CROSSBOW_SERIES) == 0)
		{
			m_csItemToolTip.AddString(ClientStr().GetStr(STI_CROSSBOW_SERIES), 12, GetTooltipColor( AGCMUIITEM_TOOLTIP_COLOR_DESCRIPTION ));
			bAddString	= TRUE;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_DRAG_BUILDUP_ITEM) == 0)
		{
			m_csItemToolTip.AddString(ClientStr().GetStr(STI_DRAG_BUILDUP_ITEM), 12, GetTooltipColor( AGCMUIITEM_TOOLTIP_COLOR_DESCRIPTION ));
			bAddString	= TRUE;
		}
		else if (strcmp(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], AGCDUIITEM_TOOLTIP_NAME_SMITH_BUILDUP_ITEM) == 0)
		{
			m_csItemToolTip.AddString(ClientStr().GetStr(STI_SMITH_BUILDUP_ITEM), 12, GetTooltipColor( AGCMUIITEM_TOOLTIP_COLOR_DESCRIPTION ));
			bAddString	= TRUE;
		}
#endif
		else
		{
			m_csItemToolTip.AddString(m_aszTooptipTitle[pcsItemTooltip->m_alTooltipIndex[i]], 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_DESCRIPTION));

			bAddString	= TRUE;
		}

		if (!bAddString)
			continue;

		m_csItemToolTip.AddNewLine(14);
	}

	AgcdItem* pcdItem = m_pcsAgcmItem->GetItemData( pcsItem );
	if( pcdItem && pcdItem->m_bHaveExtraData )
	{
		char* pFormatCharName = m_pcsAgcmUIManager2->GetUIMessage( "TextFormat_CharacterName" );
		if( pFormatCharName && strlen( pFormatCharName ) > 0 )
		{
			char strText[ 128 ] = { 0, };
			sprintf_s( strText, sizeof( char ) * 128, pFormatCharName, pcdItem->m_SealData.strCharName );

			m_csItemToolTip.AddString( strText, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
			m_csItemToolTip.AddNewLine( 14 );
		}

		AgpdCharacterTemplate* ppdCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( pcdItem->m_SealData.TID );
		if( ppdCharacterTemplate )
		{
			AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacterTemplate->m_csFactor );
			AuCharClassType eClassType = ( AuCharClassType )m_pcsAgpmFactors->GetClass( &ppdCharacterTemplate->m_csFactor );
			
			char* pFormatCharRace = m_pcsAgcmUIManager2->GetUIMessage( "TextFormat_CharacterRace" );
			if( pFormatCharRace && strlen( pFormatCharRace ) > 0 )
			{
				char* pRaceName = NULL;
				switch( eRaceType )
				{
				case AURACE_TYPE_HUMAN :		pRaceName = ClientStr().GetStr( STI_RACE_BATTLE_HUMAN );	break;
				case AURACE_TYPE_ORC :			pRaceName = ClientStr().GetStr( STI_RACE_BATTLE_ORC );		break;
				case AURACE_TYPE_MOONELF :		pRaceName = ClientStr().GetStr( STI_RACE_BATTLE_MOONELF );	break;
				case AURACE_TYPE_DRAGONSCION :	pRaceName = ClientStr().GetStr( STI_DRAGONSCION );			break;
				}

				if( pRaceName && strlen( pRaceName ) > 0 )
				{
					char strText[ 128 ] = { 0, };
					sprintf_s( strText, sizeof( char ) * 128, pFormatCharRace, pRaceName );

					m_csItemToolTip.AddString( strText, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
					m_csItemToolTip.AddNewLine( 14 );
				}
			}

			char* pFormatCharClass = m_pcsAgcmUIManager2->GetUIMessage( "TextFormat_CharacterClass" );
			if( pFormatCharClass && strlen( pFormatCharClass ) > 0 )
			{
				char* pClassName = NULL;
				switch( eRaceType )
				{
				case AURACE_TYPE_HUMAN :
					{
						switch( eClassType )
						{
						case AUCHARCLASS_TYPE_KNIGHT :	pClassName = ClientStr().GetStr( STI_KNIGHT );		break;
						case AUCHARCLASS_TYPE_RANGER :	pClassName = ClientStr().GetStr( STI_ARCHER );		break;
						case AUCHARCLASS_TYPE_MAGE :	pClassName = ClientStr().GetStr( STI_MAGE );		break;
						}
					}
					break;

				case AURACE_TYPE_ORC :
					{
						switch( eClassType )
						{
						case AUCHARCLASS_TYPE_KNIGHT :	pClassName = ClientStr().GetStr( STI_BERSERKER );	break;
						case AUCHARCLASS_TYPE_RANGER :	pClassName = ClientStr().GetStr( STI_HUNTER );		break;
						case AUCHARCLASS_TYPE_MAGE :	pClassName = ClientStr().GetStr( STI_SORCERER );	break;
						}
					}
					break;

				case AURACE_TYPE_MOONELF :
					{
						switch( eClassType )
						{
						case AUCHARCLASS_TYPE_KNIGHT :	pClassName = ClientStr().GetStr( STI_SWASHBUKLER );	break;
						case AUCHARCLASS_TYPE_RANGER :	pClassName = ClientStr().GetStr( STI_RANGER );		break;
						case AUCHARCLASS_TYPE_MAGE :	pClassName = ClientStr().GetStr( STI_ELEMETALIST );	break;
						}
					}
					break;

				case AURACE_TYPE_DRAGONSCION :
					{
						switch( eClassType )
						{
						case AUCHARCLASS_TYPE_KNIGHT :	pClassName = ClientStr().GetStr( STI_SLAYER );		break;
						case AUCHARCLASS_TYPE_RANGER :	pClassName = ClientStr().GetStr( STI_OBITER );		break;
						case AUCHARCLASS_TYPE_MAGE :	pClassName = ClientStr().GetStr( STI_SUMMERNER );	break;
						case AUCHARCLASS_TYPE_SCION :	pClassName = ClientStr().GetStr( STI_SCION );		break;
						}
					}
					break;
				}

				if( pClassName && strlen( pClassName ) > 0 )
				{
					char strText[ 128 ] = { 0, };
					sprintf_s( strText, sizeof( char ) * 128, pFormatCharClass, pClassName );

					m_csItemToolTip.AddString( strText, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
					m_csItemToolTip.AddNewLine( 14 );
				}
			}
		}

		char* pFormatCharLevel = m_pcsAgcmUIManager2->GetUIMessage( "TextFormat_CharacterLevel" );
		if( pFormatCharLevel && strlen( pFormatCharLevel ) > 0 )
		{
			char strText[ 128 ] = { 0, };
			sprintf_s( strText, sizeof( char ) * 128, pFormatCharLevel, pcdItem->m_SealData.Level );
			m_csItemToolTip.AddString( strText, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
			m_csItemToolTip.AddNewLine( 14 );
		}

		char* pFormatCharExp = m_pcsAgcmUIManager2->GetUIMessage( "TextFormat_CharacterExp" );
		if( pFormatCharExp && strlen( pFormatCharExp ) > 0 )
		{
			char strText[ 128 ] = { 0, };
			sprintf_s( strText, sizeof( char ) * 128, pFormatCharExp, pcdItem->m_SealData.Exp );
			m_csItemToolTip.AddString( strText, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
			m_csItemToolTip.AddNewLine( 14 );
		}
	}

	SetItemToolTipTimeInfo(pcsItem, pcsItemTooltip);

	return TRUE;
}

BOOL AgcmUIItem::SetItemToolTipName(AgpdItem *pcsItem, BOOL bIsNPCTradeGrid)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	char strTemp[AGCMUIITEM_MAX_TOOLTIP_TEXT_LENGTH] = {0, };

	DWORD	dwColor	= GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL);
	INT32	lItemRank	= 0;

	m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_ulTitleFontColor > 0)
		dwColor	= ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_ulTitleFontColor;
	else if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP && lItemRank == 2)
		dwColor	= GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_RARE);
	else if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP && lItemRank == 3)
		dwColor = GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_UNIQUE);
	else if (pcsItem->m_apcsOptionTemplate[0])
		//dwColor = AGCMUIITEM_TOOLTIP_COLOR_OPTION;	//	옵션있는 노멀탬은 녹색으로 나왔었는데
		dwColor=GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL);		//  요청으로 하얀색으로 변경
	else if (m_pcsAgpmItemConvert->IsEgoItem(pcsItem))
		dwColor	= GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_EGO);
	else if (m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem) > 0)
		dwColor	= GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_SPIRIT);

	// Item Name
#ifdef _DEBUG
	sprintf(strTemp, "%s (%d)", ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_szName, ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lID);
#else
	sprintf(strTemp, "%s", ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_szName);
#endif
	if (!bIsNPCTradeGrid)
	{
		// Physical Convert
		if ( m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsItem) > 0 )
			sprintf( strTemp + strlen(strTemp), " + %d", m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsItem) );

		// 마상무기가 아닌 아이템만 소켓을 표시한다.
		if ( !((((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP) &&
			 (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON) &&
			 (AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE == ((AgpdItemTemplateEquipWeapon *) pcsItem->m_pcsItemTemplate)->m_nWeaponType))
		   )
		{
			// Socket Convert
			if ( m_pcsAgpmItemConvert->GetNumSocket(pcsItem) > 0 )
				sprintf( strTemp + strlen(strTemp), " [%d/%d]", m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem), m_pcsAgpmItemConvert->GetNumSocket(pcsItem) );
		}
	}

	m_csItemToolTip.AddString( strTemp, 14, dwColor, 4 )	;

	if (m_pcsAgpmItem->IsBoundOnOwner(pcsItem))
	{
		m_csItemToolTip.AddNewLine(14);
		m_csItemToolTip.AddNewLine(14);

		m_csItemToolTip.AddString(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_BOUND_ON_OWNER), 14, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_BOUND_TYPE));
	}
	else if (m_pcsAgpmItem->GetBoundType(pcsItem) != E_AGPMITEM_NOT_BOUND)
	{
		m_csItemToolTip.AddNewLine(14);
		m_csItemToolTip.AddNewLine(14);

		switch (m_pcsAgpmItem->GetBoundType(pcsItem)) {
		case E_AGPMITEM_BIND_ON_ACQUIRE:
			m_csItemToolTip.AddString(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_BIND_ON_ACQUIRE), 14, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_BOUND_TYPE));
			break;

		case E_AGPMITEM_BIND_ON_EQUIP:
			m_csItemToolTip.AddString(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_BIND_ON_EQUIP), 14, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_BOUND_TYPE));
			break;

		case E_AGPMITEM_BIND_ON_USE:
			m_csItemToolTip.AddString(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_BIND_ON_USE), 14, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_BOUND_TYPE));
			break;
		}
	}

	if (m_pcsAgpmItem->IsQuestItem(pcsItem))
	{
		m_csItemToolTip.AddNewLine(14);
		m_csItemToolTip.AddNewLine(14);

		m_csItemToolTip.AddString(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_QUEST_TYPE), 14, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_QUEST_TYPE));
	}

	if (pcsItem->m_pcsItemTemplate->m_bIsEventItem)
	{
		m_csItemToolTip.AddNewLine(14);
		m_csItemToolTip.AddNewLine(14);

		m_csItemToolTip.AddString(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_EVENT_ITEM), 14, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_QUEST_TYPE));
	}

	/*
	if (pcsItem->m_pcsItemTemplate->m_bIsUseOnlyPCBang)
	{
		m_csItemToolTip.AddNewLine(14);
		m_csItemToolTip.AddNewLine(14);

		m_csItemToolTip.AddString(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_PCBANG_ONLY), 14, AGCMUIITEM_TOOLTIP_COLOR_QUEST_TYPE);
	}
	*/

	// 2006.06.21. steeple
	if (pcsItem->m_pcsItemTemplate->m_eBuyerType != AGPMITEM_BUYER_TYPE_ALL)
	{
		m_csItemToolTip.AddNewLine(14);
		m_csItemToolTip.AddNewLine(14);

		if(pcsItem->m_pcsItemTemplate->m_eBuyerType & AGPMITEM_BUYER_TYPE_GUILD_MASTER_ONLY)
			m_csItemToolTip.AddString(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_BUYER_TYPE_GUILD_MASTER_ONLY), 14, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_BOUND_TYPE));
	}

	// 2006.06.21. steeple
	if (pcsItem->m_pcsItemTemplate->m_lUsingType != AGPMITEM_USING_TYPE_ALL)
	{
		m_csItemToolTip.AddNewLine(14);
		m_csItemToolTip.AddNewLine(14);
		
		if(pcsItem->m_pcsItemTemplate->m_lUsingType & AGPMITEM_USING_TYPE_SIEGE_WAR_ONLY)
			m_csItemToolTip.AddString(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_USING_TYPE_SIEGE_WAR_ONLY), 14, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_BOUND_TYPE));
		
		if((pcsItem->m_pcsItemTemplate->m_lUsingType & AGPMITEM_USING_TYPE_SIEGE_WAR_ATTACK_ONLY) &&
			!(pcsItem->m_pcsItemTemplate->m_lUsingType & AGPMITEM_USING_TYPE_SIEGE_WAR_DEFENSE_ONLY))
		{
			m_csItemToolTip.AddNewLine(14);
			m_csItemToolTip.AddString(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_USING_TYPE_SIEGE_WAR_ATTACK_ONLY), 14, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_BOUND_TYPE));
		}
		else if(!(pcsItem->m_pcsItemTemplate->m_lUsingType & AGPMITEM_USING_TYPE_SIEGE_WAR_ATTACK_ONLY) &&
			(pcsItem->m_pcsItemTemplate->m_lUsingType & AGPMITEM_USING_TYPE_SIEGE_WAR_DEFENSE_ONLY))
		{
			m_csItemToolTip.AddNewLine(14);
			m_csItemToolTip.AddString(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_USING_TYPE_SIEGE_WAR_DEFENSE_ONLY), 14, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_BOUND_TYPE));
		}
	}

	AgcdUIItemTooltipDesc	*pcsAttachTooltipData	= GetADItemTemplate((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate);

	if (strlen(pcsAttachTooltipData->m_szDescription) > 0)
	{
		m_csItemToolTip.AddNewLine(14);
		m_csItemToolTip.AddNewLine(14);

		CHAR szBuffer[AGCMUIITEM_MAX_TOOLTIP_TEXT_LENGTH] = {0, };

		CHAR	*pszDescription	= pcsAttachTooltipData->m_szDescription;
		INT32	lDescLength		= strlen(pszDescription);
		INT32	lStartStringPos	= 0;

		for (int i = 0; i < lDescLength; ++i)
		{
			if (pszDescription[i] == '\\' &&
				pszDescription[i + 1] == 'n')
			{
				ZeroMemory(szBuffer, sizeof(CHAR) * AGCMUIITEM_MAX_TOOLTIP_TEXT_LENGTH);
				strncpy(szBuffer, pszDescription + lStartStringPos, i - lStartStringPos);

				m_csItemToolTip.AddString(szBuffer, 14, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));
				m_csItemToolTip.AddNewLine(14);

				lStartStringPos	= i + 2;

				++i;
			}
		}

		ZeroMemory(szBuffer, sizeof(CHAR) * AGCMUIITEM_MAX_TOOLTIP_TEXT_LENGTH);
		ASSERT(lDescLength - lStartStringPos < AGCMUIITEM_MAX_TOOLTIP_TEXT_LENGTH);
		strncpy(szBuffer, pszDescription + lStartStringPos, lDescLength - lStartStringPos);

		m_csItemToolTip.AddString( szBuffer, 14, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) )	;

		// 아바타 아이템이면 한 줄 더 띄워준다. 2007.08.21. steeple
		if(m_pcsAgpmItem->IsAvatarItem(pcsItem->m_pcsItemTemplate))
			m_csItemToolTip.AddNewLine(14);
	}

	return TRUE;
}

BOOL AgcmUIItem::SetItemToolTipPrice(AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid)
{
	if (!pcsItem || !bShowPrice)
		return FALSE;

	char				strTemp[256];
	INT32				lItemPrice;
	memset( strTemp, '\0', sizeof( strTemp ) );

	DWORD	dwColor	= GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL);

	if (m_pcsAgpmItemConvert->IsEgoItem(pcsItem))
		dwColor	= GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_EGO);
	else if (m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem) > 0)
		dwColor	= GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_SPIRIT);

	// 판매 가격 
	m_csItemToolTip.AddNewLine( 14 );
	memset( strTemp, '\0', sizeof( strTemp ) );

	//아이템 가격을 실수값으로 리턴하기때문에 INT32로 캐스팅한다.
	if (bIsNPCTradeGrid)
	{
		if( pcsItem->m_nCount > 0)
			lItemPrice = (INT32)(m_pcsAgpmItem->GetNPCPrice( pcsItem ) * pcsItem->m_nCount);
		else
			lItemPrice = (INT32)m_pcsAgpmItem->GetNPCPrice( pcsItem );
		
		AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
		INT32 lTaxRatio = m_pcsAgpmCharacter->GetTaxRatio(pcsCharacter);
		INT32 lTax = 0;
		if (lTaxRatio > 0)
		{
			lTax = (lItemPrice * lTaxRatio) / 100;
		}
		lItemPrice = lItemPrice + lTax;
	}
	else
	{
		if( pcsItem->m_nCount > 0)
			lItemPrice = (INT32)(m_pcsAgpmItem->GetPCPrice( pcsItem ) * pcsItem->m_nCount);
		else
			lItemPrice = (INT32)m_pcsAgpmItem->GetPCPrice( pcsItem );
	}
	
	sprintf( strTemp, "%s : %d %s", ClientStr().GetStr(STI_SELL_PRICE), lItemPrice, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_MONEY_NAME) );
	m_csItemToolTip.AddString( strTemp, 12, dwColor );

	return TRUE;
}

BOOL AgcmUIItem::SetItemToolTipConvertAttrInfo(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	if (pcsItem->m_apcsOptionTemplate[0])
	{
		BOOL bIsDrawTitle = FALSE;
		for (int i = 0; i < AGPDITEM_OPTION_MAX_NUM; ++i)
		{
			if (!pcsItem->m_apcsOptionTemplate[i])
				break;

			if (!pcsItem->m_apcsOptionTemplate[i]->m_bIsVisible)
				continue;

			if( !bIsDrawTitle )
			{
				CHAR* pTitle = m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_ITEM_ADDITIONALOPTION_TITLE );
				m_csItemToolTip.AddString( pTitle, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
				m_csItemToolTip.AddNewLine(14);
				bIsDrawTitle = TRUE;
			}

			m_csItemToolTip.AddString(pcsItem->m_apcsOptionTemplate[i]->m_szDescription, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_OPTION));
			m_csItemToolTip.AddNewLine(14);
		}

		m_csItemToolTip.AddNewLine(14);
	}

	if (pcsItem->m_pcsItemTemplate->m_alLinkID[0])
	{
		AgpdItemOptionTemplate* pcsItemOptionTemplate = NULL;
		for (int i = 0; i < AGPDITEM_LINK_MAX_NUM; ++i)
		{
			if(!pcsItem->m_pcsItemTemplate->m_alLinkID[i])
				break;

			pcsItemOptionTemplate = m_pcsAgpmItem->GetFitLinkTemplate(pcsItem->m_pcsItemTemplate->m_alLinkID[i],
													m_pcsAgpmCharacter->GetLevel(m_pcsAgcmCharacter->GetSelfCharacter()));
			if(!pcsItemOptionTemplate)
				continue;

			if(!pcsItemOptionTemplate->m_bIsVisible)
				continue;

			m_csItemToolTip.AddString(pcsItemOptionTemplate->m_szDescription, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_OPTION));
			m_csItemToolTip.AddNewLine(14);
		}

		m_csItemToolTip.AddNewLine(14);
	}

	if(pcsItem->m_aunSkillPlusTID[0])
	{
		// 2007.02.07. steeple
		// Skill Plus Tooltip
		for(int i = 0; i < AGPMITEM_MAX_SKILL_PLUS_EFFECT; ++i)
		{
			if(pcsItem->m_aunSkillPlusTID[i] == 0)
				break;

			AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(pcsItem->m_aunSkillPlusTID[i]);
			if(!pcsSkillTemplate)
				continue;

			CHAR szTmp[255];
			ZeroMemory(szTmp, sizeof(szTmp));
			sprintf(szTmp, "%s +Lv1", pcsSkillTemplate->m_szName);

			m_csItemToolTip.AddString(szTmp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_SKILL_PLUS));
			m_csItemToolTip.AddNewLine(14);
		}

		m_csItemToolTip.AddNewLine(14);
	}

	if (m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem) == 0)
		return TRUE;

	char strTemp[256];
	memset( strTemp, '\0', sizeof( strTemp ) );

//	if (m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsItem) > 0)
//	{
//		sprintf(strTemp, "%s +%d", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_CONVERT_PHYSICAL), m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsItem));
//		m_csItemToolTip.AddString(strTemp);
//		m_csItemToolTip.AddNewLine(14);
//	}
//
//	if (m_pcsAgpmItemConvert->GetNumSpiritStone(pcsItem) > 0)
//	{
//		sprintf(strTemp, "%s %d%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_CONVERT_SPIRITSTONE), m_pcsAgpmItemConvert->GetNumSpiritStone(pcsItem), m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_CONVERT_STEP));
//		m_csItemToolTip.AddString(strTemp);
//		m_csItemToolTip.AddNewLine(14);
//	}
//
//	if (m_pcsAgpmItemConvert->GetNumRune(pcsItem) > 0)
//	{
//		sprintf(strTemp, "%s %d%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_CONVERT_RUNE), m_pcsAgpmItemConvert->GetNumRune(pcsItem), m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_CONVERT_STEP));
//		m_csItemToolTip.AddString(strTemp);
//		m_csItemToolTip.AddNewLine(14);
//	}
//
//	m_csItemToolTip.AddNewLine(14);

	sprintf(strTemp, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_CONVERT_SOCKET_STATUS), m_pcsAgpmItemConvert->GetNumSocket(pcsItem), m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem));
	m_csItemToolTip.AddString(strTemp);
	m_csItemToolTip.AddNewLine(14);

	AgpdItemConvertADItem	*pcsAttachData	= m_pcsAgpmItemConvert->GetADItem(pcsItem);

	int i;
	for (i = 0; i < m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem); ++i)
	{
		if (!pcsAttachData->m_stSocketAttr[i].bIsSpiritStone)
			continue;

		INT32	lSpiritStoneRank	= 0;
		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lSpiritStoneRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

		INT32	lAddValue			= 0;
		if (m_pcsAgpmItemConvert->IsWeapon(pcsItem))
		{
			lAddValue			= m_pcsAgpmItemConvert->m_astTableSpiritStone[lSpiritStoneRank].lWeaponAddValue;
			sprintf(strTemp, "%s + %d", pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_szName, lAddValue);
		}
		else
		{
			lAddValue			= m_pcsAgpmItemConvert->m_astTableSpiritStone[lSpiritStoneRank].lArmourAddValue;
			sprintf(strTemp, "%s + %d%%", pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_szName, lAddValue);
		}

		INT32	lAttrMagicDmg	= 0;
		INT32	lAttrWaterDmg	= 0;
		INT32	lAttrFireDmg	= 0;
		INT32	lAttrEarthDmg	= 0;
		INT32	lAttrAirDmg		= 0;

		INT32	lAttrMagicDef	= 0;
		INT32	lAttrWaterDef	= 0;
		INT32	lAttrFireDef	= 0;
		INT32	lAttrEarthDef	= 0;
		INT32	lAttrAirDef		= 0;

		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lAttrMagicDmg, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lAttrWaterDmg, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lAttrFireDmg, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lAttrEarthDmg, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lAttrAirDmg, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);

		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lAttrMagicDef, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lAttrWaterDef, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lAttrFireDef, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lAttrEarthDef, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lAttrAirDef, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);

		if (lAttrMagicDmg > 0 || lAttrMagicDef > 0)
			m_csItemToolTip.AddString(strTemp, 12, 0xffb136ff);
		else if (lAttrWaterDmg > 0 || lAttrWaterDef > 0)
			m_csItemToolTip.AddString(strTemp, 12, 0xff0078ff);
		else if (lAttrFireDmg > 0 || lAttrFireDef > 0)
			m_csItemToolTip.AddString(strTemp, 12, 0xffff3710);
		else if (lAttrEarthDmg > 0 || lAttrEarthDef > 0)
			m_csItemToolTip.AddString(strTemp, 12, 0xffe0c867);
		else if (lAttrAirDmg > 0 || lAttrAirDef > 0)
			m_csItemToolTip.AddString(strTemp, 12, 0xff7cff38);
		else
			m_csItemToolTip.AddString(strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_SPIRIT));

		m_csItemToolTip.AddNewLine(14);
	}

	m_csItemToolTip.AddNewLine(14);

	for (i = 0; i < m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem); ++i)
	{
		if (!pcsAttachData->m_stSocketAttr[i].pcsItemTemplate ||
			pcsAttachData->m_stSocketAttr[i].bIsSpiritStone)
			continue;

		AgpdItemTemplate* ppdSocketItemTemplate = pcsAttachData->m_stSocketAttr[i].pcsItemTemplate;

		INT32 nOptionTIDIndex = 0;
		INT32 nOptionTID = ppdSocketItemTemplate->m_alOptionTID[ nOptionTIDIndex ];
		while( nOptionTID )
		{
			AgpdItemOptionTemplate* ppdItemOptionTemplate = m_pcsAgpmItem->GetItemOptionTemplate( nOptionTID );
			if( ppdItemOptionTemplate )
			{
				if( ppdItemOptionTemplate->m_szDescription && ppdItemOptionTemplate->m_szDescription[ 0 ] )
				{
					sprintf( strTemp, "%s", ppdItemOptionTemplate->m_szDescription );
					m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_RUNE) );
					m_csItemToolTip.AddNewLine(14);
				}
			}

			nOptionTIDIndex++;
			nOptionTID = ppdSocketItemTemplate->m_alOptionTID[ nOptionTIDIndex ];
		}
	}

	return TRUE;
}

BOOL AgcmUIItem::SetItemConvertIcon(AgpdItem *pcsItem, INT32 lMaxSocket)
{
	if (!pcsItem || lMaxSocket < 1)
		return FALSE;

	m_csItemToolTip.AddNewLine(16);

	AgpdItemConvertADItem	*pcsAttachData	= m_pcsAgpmItemConvert->GetADItem(pcsItem);

	INT32	lIndex	= 0;

	BOOL	bLastAddNewLine	= FALSE;

	//for (int i = 0; i < lMaxSocket; ++i)
	for (int i = 0; i < pcsAttachData->m_lNumSocket; ++i)
	{
		//if (pcsAttachData->m_lNumSocket > i)
		{
			if (pcsAttachData->m_lNumConvert > i && pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)
			{
				AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsAttachData->m_stSocketAttr[i].pcsItemTemplate);

				if (!pcsAgcdItemTemplate->m_pTexture)
				{
					if (m_pcsAgcmItem->m_pcsAgcmResourceLoader)
						m_pcsAgcmItem->m_pcsAgcmResourceLoader->SetTexturePath(m_pcsAgcmItem->m_szIconTexturePath);

					//pcsAgcdItemTemplate->m_pTexture	= RwTextureRead(pcsAgcdItemTemplate->m_szTextureName, NULL);

					if (pcsAgcdItemTemplate->m_pcsPreData)
					{
						pcsAgcdItemTemplate->m_pTexture	= RwTextureRead(pcsAgcdItemTemplate->m_pcsPreData->m_pszTextureName, NULL);
					}
					else
					{
						CHAR	szTemp[256];
						memset(szTemp, 0, sizeof (CHAR) * 256);

						if (pcsAgcdItemTemplate->GetTextureID() > 0)
						{
							pcsAgcdItemTemplate->GetTextureName(szTemp);

							if (szTemp && szTemp[0])
								pcsAgcdItemTemplate->m_pTexture	= m_pcsAgcmItem->GetTexture(szTemp);
						}
					}
				}

				if (pcsAgcdItemTemplate->m_pTexture)
				{
					m_csItemToolTip.AddToolTipTexture(pcsAgcdItemTemplate->m_pTexture, 50, 10 + 54 * lIndex);
				}

				if (m_pcsAgcmUIControl->m_pConvertOutline)
				{
					m_csItemToolTip.AddToolTipTexture(m_pcsAgcmUIControl->m_pConvertOutline, 50, 10 + 54 * lIndex, 0xffffffff, TRUE);
				}
			}
			else
			{
				if (m_pcsAgcmUIControl->m_pConvertBase)
				{
					m_csItemToolTip.AddToolTipTexture(m_pcsAgcmUIControl->m_pConvertBase, 50, 10 + 54 * lIndex);
				}
			}
		}
		//else
		//{
		//	if (m_pcsAgcmUIControl->m_pConvertDisable)
		//	{
		//		m_csItemToolTip.AddToolTipTexture(m_pcsAgcmUIControl->m_pConvertDisable, 50, 10 + 54 * lIndex);
		//	}
		//}

		++lIndex;

//				AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate);
//
//				if (pcsAgcdItemTemplate->m_pTexture)
//					m_csItemToolTip.AddToolTipTexture(pcsAgcdItemTemplate->m_pTexture, 64, 10 + 50 * i);

		if (lIndex == 4)
		{
			m_csItemToolTip.AddNewLine(54);
			lIndex	= 0;
			bLastAddNewLine	= TRUE;
		}
		else
			bLastAddNewLine	= FALSE;
	}

	if (!bLastAddNewLine)
		m_csItemToolTip.AddNewLine(54);

	return TRUE;
}

BOOL AgcmUIItem::SetItemToolTipSpiritStoneInfo(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	CHAR	strTemp[64];

	INT32	lFire	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lFire, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);

	if (lFire > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DAMAGE_FIRE), lFire );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
		m_csItemToolTip.AddNewLine( 12 );

		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DEFENSE_FIRE), lFire );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	INT32	lWater	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lWater, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);

	if (lWater > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DAMAGE_WATER), lWater );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
		m_csItemToolTip.AddNewLine( 12 );

		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DEFENSE_WATER), lWater );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	INT32	lAir	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lAir, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);

	if (lAir > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DAMAGE_AIR), lAir );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
		m_csItemToolTip.AddNewLine( 12 );

		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DEFENSE_AIR), lAir );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	INT32	lEarth	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lEarth, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);

	if (lEarth > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DAMAGE_EARTH), lEarth );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
		m_csItemToolTip.AddNewLine( 12 );

		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DEFENSE_EARTH), lEarth );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	INT32	lMagic	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lMagic, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);

	if (lMagic > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DAMAGE_MAGIC), lMagic );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
		m_csItemToolTip.AddNewLine( 12 );

		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DEFENSE_MAGIC), lMagic );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	INT32	lPoison	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lPoison, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON);

	if (lPoison > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DAMAGE_POISON), lPoison );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	INT32	lLightening	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lLightening, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING);

	if (lLightening > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DAMAGE_LIGHTENING), lLightening );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	INT32	lIce	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lIce, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE);

	if (lIce > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DAMAGE_ICE), lIce );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	INT32	lStr	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lStr, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);

	if (lStr > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DEFENSE_STR), lStr );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	INT32	lCon	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lCon, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);

	if (lCon > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DEFENSE_CON), lCon );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	INT32	lInt	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lInt, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);

	if (lInt > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DEFENSE_INT), lInt );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	INT32	lWis	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lWis, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);

	if (lWis > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DEFENSE_WIS), lWis );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	INT32	lDex	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lDex, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);

	if (lDex > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_SS_DEFENSE_DEX), lDex );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );
	}

	return TRUE;
}

BOOL AgcmUIItem::SetItemToolTipRingInfo(AgpdItem *pcsItem, AgcdUIItemTooltip *pcsItemTooltip)
{
	if (!pcsItem || !pcsItemTooltip)
		return FALSE;

	CHAR	strTemp[64];

	BOOL	bIsAddedLine	= FALSE;

	INT32	lStr	= 0;
	m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lStr, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);

	if (lStr > 0)
	{
		sprintf( strTemp, "%s : +%d", ClientStr().GetStr(STI_STR), lStr );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

		//if (bIsAddedLine)
			m_csItemToolTip.AddNewLine(12);

		bIsAddedLine	= TRUE;
	}

	INT32	lCon	= 0;
	m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lCon, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);

	if (lCon > 0)
	{
		sprintf( strTemp, "%s : +%d", ClientStr().GetStr(STI_CON), lCon );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

		if (bIsAddedLine)
			m_csItemToolTip.AddNewLine(12);

		bIsAddedLine	= TRUE;
	}

	INT32	lInt	= 0;
	m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lInt, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);

	if (lInt > 0)
	{
		sprintf( strTemp, "%s : +%d", ClientStr().GetStr(STI_INT), lInt );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

		if (bIsAddedLine)
			m_csItemToolTip.AddNewLine(12);

		bIsAddedLine	= TRUE;
	}

	INT32	lWis	= 0;
	m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lWis, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);

	if (lWis > 0)
	{
		sprintf( strTemp, "%s : +%d", ClientStr().GetStr(STI_WIS), lWis );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

		if (bIsAddedLine)
			m_csItemToolTip.AddNewLine(12);

		bIsAddedLine	= TRUE;
	}

	INT32	lDex	= 0;
	m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lDex, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);

	if (lDex > 0)
	{
		sprintf( strTemp, "%s : +%d", ClientStr().GetStr(STI_DEX), lDex );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

		if (bIsAddedLine)
			m_csItemToolTip.AddNewLine(12);

		bIsAddedLine	= TRUE;
	}

	return TRUE;
}

BOOL AgcmUIItem::SetItemToolTipAmuletInfo(AgpdItem *pcsItem, AgcdUIItemTooltip *pcsItemTooltip)
{
	if (!pcsItem || !pcsItemTooltip)
		return FALSE;

	CHAR	strTemp[64];

	BOOL	bIsAddedLine	= FALSE;

	INT32	lFireDefense	= 0;
	m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lFireDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);

	if (lFireDefense > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_DEFENSE_FIRE), lFireDefense );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

		//if (bIsAddedLine)
			m_csItemToolTip.AddNewLine(12);

		bIsAddedLine	= TRUE;
	}

	INT32	lWaterDefense	= 0;
	m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lWaterDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);

	if (lWaterDefense > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_DEFENSE_WATER), lWaterDefense );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

		if (bIsAddedLine)
			m_csItemToolTip.AddNewLine(12);

		bIsAddedLine	= TRUE;
	}

	INT32	lAirDefense	= 0;
	m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lAirDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);

	if (lAirDefense > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_DEFENSE_AIR), lAirDefense );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

		if (bIsAddedLine)
			m_csItemToolTip.AddNewLine(12);

		bIsAddedLine	= TRUE;
	}

	INT32	lEarthDefense	= 0;
	m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lEarthDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);

	if (lEarthDefense > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_DEFENSE_EARTH), lEarthDefense );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

		if (bIsAddedLine)
			m_csItemToolTip.AddNewLine(12);

		bIsAddedLine	= TRUE;
	}

	INT32	lMagicDefense	= 0;
	m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lMagicDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);

	if (lMagicDefense > 0)
	{
		sprintf( strTemp, "%s : +%d %%", ClientStr().GetStr(STI_DEFENSE_MAGIC), lMagicDefense );
		m_csItemToolTip.AddString( strTemp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL) );

		if (bIsAddedLine)
			m_csItemToolTip.AddNewLine(12);

		bIsAddedLine	= TRUE;
	}

	return TRUE;
}

// 2008.06.16. steeple
BOOL AgcmUIItem::SetItemToolTipTimeInfo(AgpdItem* pcsItem, AgcdUIItemTooltip* pcsItemTooltip)
{
	if(!pcsItem || !pcsItemTooltip)
		return FALSE;

	CHAR szTmp[255];
	ZeroMemory(szTmp, sizeof(szTmp));
	BOOL bAddLine = FALSE;

	EnumCallback(AGCMUIITEM_CB_GET_ITEM_TIME_INFO, pcsItem, szTmp);

	// 아이템의 종료시간. CashItem 모듈에서 가져온다.
	if(_tcslen(szTmp) > 0)
	{
		m_csItemToolTip.AddString(szTmp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));
		bAddLine = TRUE;
	}

	// Stamina 찍어준다.
	if(pcsItem->m_pcsItemTemplate->m_llStaminaRemainTime > 0)
	{
		ZeroMemory(szTmp, sizeof(szTmp));
		EnumCallback(AGCMUIITEM_CB_GET_ITEM_STAMINA_TIME, pcsItem, szTmp);

		if(_tcslen(szTmp) > 0)
		{
			if(bAddLine)
				m_csItemToolTip.AddNewLine(12);

			m_csItemToolTip.AddString(szTmp, 12, GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));
			bAddLine = TRUE;
		}
	}

	if(bAddLine)
		m_csItemToolTip.AddNewLine(12);

	return TRUE;
}

CHAR* AgcmUIItem::_GetItemSecondCategoryName( AgpdItemTemplate *pcsItemTemplate )
{
	if( m_pcsAgpmAuctionCategory )
	{
		AgpdAuctionCategory2Info** ppstSecondCategory = ( AgpdAuctionCategory2Info** )m_pcsAgpmAuctionCategory->GetCategory2()->GetObject( pcsItemTemplate->m_lSecondCategory );

		if( ppstSecondCategory && *ppstSecondCategory )
		{
			return ( *ppstSecondCategory )->m_strName;
		}
	}

	return NULL;
}

BOOL AgcmUIItem::_CollectClassNames( CHAR* pBuffer, INT32 nBufferSize, AuCharClassType eClasType, INT32 nEnableRaces )
{
	if( !pBuffer ) return FALSE;
	memset( pBuffer, 0, nBufferSize );

	bool bIsFirst = true;
	switch( eClasType )
	{
	case AUCHARCLASS_TYPE_KNIGHT :
		{
			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_HUMAN ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_KNIGHT ) );
				bIsFirst = false;
			}

			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_ORC ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_BERSERKER ) );
				bIsFirst = false;
			}

			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_MOONELF ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_SWASHBUCKLER ) );
				bIsFirst = false;
			}

			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_DRAGONSCION ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_SLAYER ) );
				bIsFirst = false;
			}
		}
		break;

	case AUCHARCLASS_TYPE_RANGER :
		{
			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_HUMAN ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_ARCHER ) );
				bIsFirst = false;
			}

			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_ORC ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_HUNTER ) );
				bIsFirst = false;
			}

			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_MOONELF ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_RANGER ) );
				bIsFirst = false;
			}

			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_DRAGONSCION ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_OBITER ) );
				bIsFirst = false;
			}
		}
		break;

	case AUCHARCLASS_TYPE_MAGE :
		{
			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_HUMAN ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_MAGE ) );
				bIsFirst = false;
			}

			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_ORC ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_SORCERER ) );
				bIsFirst = false;
			}

			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_MOONELF ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_ELEMENTALIST ) );
				bIsFirst = false;
			}

			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_DRAGONSCION ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_SUMMERNER ) );
				bIsFirst = false;
			}
		}
		break;

	case AUCHARCLASS_TYPE_SCION :
		{
			if( _IsEnableRace( nEnableRaces, AURACE_TYPE_DRAGONSCION ) )
			{
				CHAR* pFormat = bIsFirst ? " %s" : " / %s";
				sprintf( pBuffer + strlen( pBuffer ), pFormat, m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_LG_CLASS_SCION ) );
				bIsFirst = false;
			}
		}
		break;
	}

	return TRUE;
}

BOOL AgcmUIItem::_IsEnableRace( INT32 nFactorCode, AuRaceType eRaceType )
{
	// 0은 모든 종족을 의미한다.
	if( nFactorCode == 0 ) return TRUE;
	return nFactorCode & ( 0x00000001 << ( eRaceType - 1 ) );
}

BOOL AgcmUIItem::_IsEnableClass( INT32 nFactorCode, AuCharClassType eClassType )
{
	// 0은 모든 클래스를 의미한다.
	if( nFactorCode == 0 ) return TRUE;
	return nFactorCode & ( 0x00000001 << ( eClassType - 1 ) );
}

VOID	AgcmUIItem::_InitTooltipColor( VOID )
{
	AgcmResourceLoader*	pLoader	=	static_cast< AgcmResourceLoader* >(GetModule( "AgcmResourceLoader" ));

	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_NORMAL		]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_NORMAL");
	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_SPIRIT		]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_SPIRIT");
	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_RUNE			]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_RUNE");
	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_EGO			]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_EGO");
	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_EQUIP		]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_EQUIP");
	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY	]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_LACK_ABILITY");
	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_DESCRIPTION	]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_DESCRIPTION");
	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_BOUND_TYPE	]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_BOUND_TYPE");
	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_QUEST_TYPE	]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_QUEST_TYPE");
	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_OPTION		]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_OPTION");
	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_RARE			]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_RARE");
	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_UNIQUE		]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_UNIQUE");
	m_dwTooltipColor[ AGCMUIITEM_TOOLTIP_COLOR_SKILL_PLUS	]	=	pLoader->GetColor("ITEM_TOOLTIP_COLOR_SKILL_PLUS");

}

DWORD AgcmUIItem::GetToolTipColor( AgpdItem* ppdItem, eAgpdFactorsType eType, int nType2, int nType3 )
{
	if( !ppdItem || !ppdItem->m_pcsItemTemplate ) return 0xFFFFFFFF;
	if( !m_pcsAgpmFactors ) return 0xFFFFFFFF;

	int nValueTemplate = 0;
	m_pcsAgpmFactors->GetValue( &ppdItem->m_pcsItemTemplate->m_csFactor, &nValueTemplate, eType, nType2, nType3 );

	int nValueInstance = 0;
	m_pcsAgpmFactors->GetValue( &ppdItem->m_csFactor, &nValueInstance, eType, nType2, nType3 );

	if( nValueTemplate > nValueInstance ) return GetTooltipColor( AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY );
	if( nValueTemplate < nValueInstance ) return GetTooltipColor( AGCMUIITEM_TOOLTIP_COLOR_SPIRIT );
	return GetTooltipColor( AGCMUIITEM_TOOLTIP_COLOR_NORMAL );
}
