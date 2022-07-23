/*==============================================================

	AgcmUIRefinery.cpp

==============================================================*/

#include "AgcmUIRefinery.h"
#include "AuStrTable.h"

/********************************************************/
/*		The Implementation of AgcmUIRefinery class		*/
/********************************************************/
//
//	Constants
//=================================
//
CHAR AgcmUIRefinery::s_szEvent[AGCMUIREFINERY_EVENT_MAX][30] = 
	{
	"REFINE_EVENT_OPEN",
	"REFINE_EVENT_CLOSE",
	"REFINE_EVENT_REFINE",
	"REFINE_EVENT_RESET",
	"REFINE_EVENT_SUCCESS",
    "REFINE_EVENT_END",
    "REFINE_EVENT_MONEYMSGBOX"
	};


CHAR AgcmUIRefinery::s_szMessage[AGCMUIREFINERY_MESSAGE_MAX][30] = 
	{
	"REFINE_MAIN_EDIT",
	"REFINE_SUB_TITLE",
	"REFINE_SUB_NOTIFY",
	"REFINE_SUCCESS",
	"REFINE_INSITEM",
	"REFINE_DIFITEM",
	"REFINE_INSMONEY",
	"REFINE_UNABLELOAD",
	"REFINE_UNREFINABLE",
	"REFINE_FULLINVEN",
	"REFINE_SATISFYQTY",
	"REFINE_SAMEATTR",
	"REFINE_INSOPT",
	"REFINE_REFINABLE",
	"REFINE_UNNEXT"
	};

// commented by st
//static CHAR s_szMain[512] = APS_REFINERY_INTRO;


AgcmUIRefinery::AgcmUIRefinery()
{
	SetModuleName("AgcmUIRefinery");

	m_pAgpmFactors			= NULL;
	m_pAgpmGrid				= NULL;
	m_pAgpmCharacter		= NULL;
	m_pAgpmItem				= NULL;
	m_pAgpmRefinery			= NULL;
	m_pAgcmCharacter		= NULL;
	m_pAgcmItem				= NULL;
	m_pAgcmRefinery			= NULL;
	m_pAgcmEventRefinery	= NULL;
	m_pAgcmChatting			= NULL;
	m_pAgcmUIManager2		= NULL;
	m_pAgcmUIControl		= NULL;
	m_pAgcmUIItem			= NULL;
	m_pAgcmUICooldown		= NULL;

	// user data
	m_pstActiveTab			= NULL;
	m_pstMainEdit			= NULL;
	m_pstSubTitle			= NULL;
	m_pstSubNotify			= NULL;
	m_pstPrice				= NULL;
	m_pstSourceItem			= NULL;
	m_pstResultItem			= NULL;
	m_pstActiveRefineButton = NULL;
	m_lDummy				= 0;
	m_lRefineButton			= 0;
	
	m_eCategory = AGPMREFINERY_CATEGORY_ITEM;
	m_eResult = AGPMREFINERY_RESULT_NONE;
	m_pSources.MemSetAll();
	m_pAgpdRefineTemplate = NULL;
	ZeroMemory(&m_stOpenPos, sizeof(AuPOS));
	m_pAgpdGridItemResult = NULL;	
	m_eNotifyMsg = AGCMUIREFINERY_MESSAGE_SUB_NOTIFY;
	m_pszNotify = NULL;
	m_bHold = FALSE;
	m_bIsOpen = FALSE;
	m_lCooldownID = AGCMUICOOLDOWN_INVALID_ID;
}

AgcmUIRefinery::~AgcmUIRefinery()
{
}

//	ApModule inherited
//===============================================
//
BOOL AgcmUIRefinery::OnInit()
{
	// initialize tooltip
	m_csToolTip.m_Property.bTopmost = TRUE;
	m_pAgcmUIManager2->AddWindow((AgcWindow *) (&m_csToolTip));
	m_csToolTip.ShowWindow(FALSE);
	
	// initialize grid item
	return InitializeGrid();
}


BOOL AgcmUIRefinery::OnDestroy()
{
	Reset(FALSE);
	DestroyGrid();
	return TRUE;
}


BOOL AgcmUIRefinery::OnAddModule()
	{
	m_pAgpmFactors		= (AgpmFactors *) GetModule("AgpmFactors");
	m_pAgpmGrid			= (AgpmGrid *) GetModule("AgpmGrid");	
	m_pAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmItem			= (AgpmItem *) GetModule("AgpmItem");
	m_pAgpmRefinery		= (AgpmRefinery *) GetModule("AgpmRefinery");
	m_pAgcmCharacter	= (AgcmCharacter *) GetModule("AgcmCharacter");
	m_pAgcmItem			= (AgcmItem *) GetModule("AgcmItem");
	m_pAgcmRefinery		= (AgcmRefinery *) GetModule("AgcmRefinery");
	m_pAgcmEventRefinery= (AgcmEventRefinery *) GetModule("AgcmEventRefinery");	
	m_pAgcmChatting		= (AgcmChatting2 *) GetModule("AgcmChatting2");
	m_pAgcmUIManager2	= (AgcmUIManager2 *) GetModule("AgcmUIManager2");
	m_pAgcmUIControl	= (AgcmUIControl *) GetModule("AgcmUIControl");
	m_pAgcmUIMain		= (AgcmUIMain *) GetModule("AgcmUIMain");
	m_pAgcmUIItem		= (AgcmUIItem *) GetModule("AgcmUIItem");
	m_pAgcmUICooldown	= (AgcmUICooldown *) GetModule("AgcmUICooldown");

	if (!m_pAgpmFactors ||!m_pAgpmGrid || !m_pAgpmCharacter ||
		!m_pAgpmItem || !m_pAgpmRefinery || !m_pAgcmCharacter ||
		!m_pAgcmItem || !m_pAgcmRefinery || !m_pAgcmEventRefinery ||
		!m_pAgcmChatting || !m_pAgcmUIManager2 || !m_pAgcmUIControl
		)
		return FALSE;
	
	// event grant callback
	if (!m_pAgcmEventRefinery->SetCallbackGrant(CBGrant, this))
		return FALSE;
	
	if (!m_pAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;
		
	// result callback
	if (!m_pAgpmRefinery->SetCallbackResult(CBRefineResult, this))
		return FALSE;
	
	if (m_pAgcmUIMain && !m_pAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	if (m_pAgcmUIMain && !m_pAgcmUIMain->SetCallbackCloseAllUITooltip(CBCloseAllUIToolTip, this))
		return FALSE;

	if (m_pAgpmItem && !m_pAgpmItem->SetCallbackRemoveFromInventory(CBRemoveItemFromInventory, this))
		return FALSE;
	if (m_pAgpmItem && !m_pAgpmItem->SetCallbackRemoveItemCashInventory(CBRemoveItemFromInventory, this))
		return FALSE;

	if (!m_pAgcmItem->SetCallbackCheckUseItem(CBCheckUseItem, this))
		return FALSE;

	if (m_pAgcmUICooldown)
		{
		m_lCooldownID = m_pAgcmUICooldown->RegisterCooldown(CBAfterCooldown, this);
		if (AGCMUICOOLDOWN_INVALID_ID >= m_lCooldownID)
			return FALSE;
		}

	if (!AddEvent() || !AddFunction() || !AddDisplay() || !AddUserData() )/*|| !AddBoolean())*/
		return FALSE;

	return TRUE;
	}




//	Operations
//========================================================
//
BOOL AgcmUIRefinery::Open()
	{
	// initialize
	Reset();

	// open window
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIREFINERY_EVENT_OPEN]);

	return TRUE;
	}


BOOL AgcmUIRefinery::OnResult(INT32 eResult, INT32 lResultItemTID)
	{
	m_eResult = eResult;
	m_bHold = FALSE;

	switch (eResult)
		{
		case AGPMREFINERY_RESULT_SUCCESS :
			OnSuccess(lResultItemTID);
			break;
			
		case AGPMREFINERY_RESULT_FAIL :
            m_eNotifyMsg = AGCMUIREFINERY_MESSAGE_RESULT_FAIL_INSUFFICIENT_ITEM;
			AddSystemMessage(GetMessageTxt(m_eCategory, AGCMUIREFINERY_MESSAGE_RESULT_FAIL_INSUFFICIENT_ITEM));
			break;
			
		case AGPMREFINERY_RESULT_FAIL_DIFFERENT_ITEM :
			m_eNotifyMsg = AGCMUIREFINERY_MESSAGE_RESULT_FAIL_DIFFRENT_ITEM;
			AddSystemMessage(GetMessageTxt(m_eCategory, AGCMUIREFINERY_MESSAGE_RESULT_FAIL_DIFFRENT_ITEM));
			break;
			
		case AGPMREFINERY_RESULT_FAIL_INSUFFICIENT_ITEM :
			m_eNotifyMsg = AGCMUIREFINERY_MESSAGE_RESULT_FAIL_INSUFFICIENT_ITEM;
			AddSystemMessage(GetMessageTxt(m_eCategory, AGCMUIREFINERY_MESSAGE_RESULT_FAIL_INSUFFICIENT_ITEM));
			break;
			
		case AGPMREFINERY_RESULT_FAIL_INSUFFICIENT_MONEY :
			m_eNotifyMsg = AGCMUIREFINERY_MESSAGE_RESULT_FAIL_INSUFFICIENT_MONEY;
			AddSystemMessage(GetMessageTxt(m_eCategory, AGCMUIREFINERY_MESSAGE_RESULT_FAIL_INSUFFICIENT_MONEY));
			break;
			
		case AGPMREFINERY_RESULT_FAIL_FULL_INVENTORY :
			m_eNotifyMsg = AGCMUIREFINERY_MESSAGE_RESULT_FAIL_FULL_INVENTORY;
			AddSystemMessage(GetMessageTxt(m_eCategory, AGCMUIREFINERY_MESSAGE_RESULT_FAIL_FULL_INVENTORY));
			break;
		
		default:
			break;
		}
	
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstSubNotify);
	
	return TRUE;
	}


void AgcmUIRefinery::OnSuccess(INT32 lResultItemTID)
	{
	AgpdItemTemplate *pAgpdItemTemplate = m_pAgpmItem->GetItemTemplate(lResultItemTID);
	if (!pAgpdItemTemplate)
		return;

	Reset();
	AddItemToResultGrid(pAgpdItemTemplate, 1);
	RefreshResultGrid();

	m_eResult = AGPMREFINERY_RESULT_SUCCESS;
	m_eNotifyMsg = AGCMUIREFINERY_MESSAGE_RESULT_SUCCESS;
	// system message
	CHAR *psz = GetMessageTxt(m_eCategory, AGCMUIREFINERY_MESSAGE_RESULT_SUCCESS);
	if (psz)
		{
		CHAR sz[512];
		sprintf(sz, psz, pAgpdItemTemplate->m_szName.c_str());
		m_pszNotify = (CHAR*)pAgpdItemTemplate->m_szName.c_str();
		AddSystemMessage(sz);
		}
	
	// change result texture(effect)
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIREFINERY_EVENT_SUCCESS]);
	}

BOOL AgcmUIRefinery::OnMoveItem(AgpdItem *pAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn)
{
    if (AGPMREFINERY_RESULT_NONE != m_eResult)
        Reset();

    // it must be in inventory
    if (AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus &&
        AGPDITEM_STATUS_CASH_INVENTORY != pAgpdItem->m_eStatus)
        return FALSE;

    // check if this dropped before
    for (INT16 i=0; i<AGPMREFINERY_MAX_GRID; ++i)
    {
        if (m_pSources[i] && pAgpdItem == m_pSources[i])
        {
            return FALSE;
        }
    }

    // grid full	
    if (m_pAgpmGrid->IsFullGrid(&m_AgpdGridSource))
    {
        m_eNotifyMsg = AGCMUIREFINERY_MESSAGE_RESULT_FAIL_UNABLE_TO_LOAD;
        AddSystemMessage(GetMessageTxt(m_eCategory, AGCMUIREFINERY_MESSAGE_RESULT_FAIL_UNABLE_TO_LOAD));
        return FALSE;
    }

    // is empty grid?
    if (!m_pAgpmGrid->IsEmptyGrid(&m_AgpdGridSource, nLayer, nRow, nColumn, 1, 1))
    {
        return FALSE;
    }

    // Item Template validate

    INT32 lDroppedItemTID = ((AgpdItemTemplate *) pAgpdItem->m_pcsItemTemplate)->m_lID;

    // add to grid
    AddItemToSourceGrid(pAgpdItem, nLayer, nRow, nColumn);

    // save dropped item
    m_pSources[(nRow * 5) + nColumn] = pAgpdItem;

    RefreshSourceGrid();

    return TRUE;
}

//	Function Callbacks
//=========================================================
//
BOOL AgcmUIRefinery::CBOpenWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	AgcmUIRefinery	*pThis = (AgcmUIRefinery *) pClass;
	AgcdUIControl	*pcsControl = (AgcdUIControl *) pData1;

	pThis->m_bIsOpen = TRUE;
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstMainEdit);
	pThis->m_eCategory = AGPMREFINERY_CATEGORY_ITEM;
	pThis->m_pstActiveTab->m_lSelectedIndex = 0;
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstSubTitle);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstSubNotify);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstActiveTab);

	return TRUE;
	}


BOOL AgcmUIRefinery::CBCloseWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIRefinery	*pThis = (AgcmUIRefinery *)	pClass;
	
	if (pThis->m_bHold)
		return TRUE;
	
	pThis->m_bIsOpen = FALSE;
	pThis->Reset(FALSE);
	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIREFINERY_EVENT_CLOSE]);
	
	return TRUE;	
	}

BOOL AgcmUIRefinery::CBClickRefineButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIRefinery *pThis = (AgcmUIRefinery *)	pClass;

	if (pThis->m_bHold)
		return FALSE;

	if (AGPMREFINERY_RESULT_NONE != pThis->m_eResult)
	{
		pThis->Reset();
		return FALSE;
	}

    BOOL bResult = FALSE;
    INT32 lResult = AGPMREFINERY_RESULT_NONE;

    bResult = pThis->m_pAgpmRefinery->IsValidStatusRefine(pThis->m_pAgcmCharacter->GetSelfCharacter(),
												    &pThis->m_pSources[0],
												    &lResult);

	if (FALSE == bResult)
	{
		pThis->OnResult(lResult, 0);
        pThis->Reset();
		return FALSE;
	}

    pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIREFINERY_EVENT_REFINEMONEYMSGBOX_OPEN]);
    return TRUE;
}

//정제 비용 알림 메시지 박스 열기.
BOOL AgcmUIRefinery::CBRefine(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
    if( !pClass )		return FALSE;

    if( lTrueCancel )
    {
        AgcmUIRefinery *pThis = (AgcmUIRefinery *)	pClass;

        pThis->m_bHold = TRUE;

        if( !pThis->m_pAgpmCharacter->SubMoney(pThis->m_pAgcmCharacter->GetSelfCharacter(), pThis->m_pAgpmRefinery->m_pAgpdResultItem->m_lPrice) )
        {
            pThis->OnResult(AGPMREFINERY_RESULT_FAIL_INSUFFICIENT_MONEY ,0);
            return FALSE;
        }

        if (pThis->m_pAgcmUICooldown && AGCMUICOOLDOWN_INVALID_ID != pThis->m_lCooldownID)
            return pThis->m_pAgcmUICooldown->StartCooldown(pThis->m_lCooldownID, 5000, ClientStr().GetStr(STI_REFINERYING));
        //else
        //{
        //    INT32 lItems[AGPMREFINERY_MAX_GRID];
        //    for (INT16 i = 0; i<AGPMREFINERY_MAX_GRID; ++i)
        //    {
        //        if (pThis->m_pSources[i])
        //            lItems[i] = pThis->m_pSources[i]->m_lID;
        //        else
        //            lItems[i] = 0;
        //    }		

        //    return pThis->m_pAgcmRefinery->SendPacketRefine(pThis->m_pAgcmCharacter->GetSelfCharacter()->m_lID,
        //        pThis->m_pAgpdRefineTemplate->m_lItemTID,
        //        lItems);
        //}
    }

    return TRUE;
}

BOOL AgcmUIRefinery::CBClickResetButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIRefinery *pThis = (AgcmUIRefinery *)	pClass;

	if (!pThis->m_bHold)
		pThis->Reset();

	return TRUE;
	}


BOOL AgcmUIRefinery::CBMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl ||
		!pcsSourceControl->m_pcsBase ||
		AcUIBase::TYPE_GRID != pcsSourceControl->m_lType
		)
		return FALSE;

	AgcmUIRefinery		*pThis					= (AgcmUIRefinery *) pClass;
	AcUIGrid			*pAcUIGrid				= (AcUIGrid *) pcsSourceControl->m_pcsBase;
	AgpdGridSelectInfo	*pAgpdGridSelectInfo	= pAcUIGrid->GetDragDropMessageInfo();

	if (!pAgpdGridSelectInfo || !pAgpdGridSelectInfo->pGridItem ||
		 AGPDGRID_ITEM_TYPE_ITEM != pAgpdGridSelectInfo->pGridItem->m_eType)
		return FALSE;

	if (pThis->m_bHold)
		return FALSE;

	AgpdItem *pAgpdItem = pThis->m_pAgpmItem->GetItem(pAgpdGridSelectInfo->pGridItem->m_lItemID);
	if (!pAgpdItem)
		return FALSE;

	pThis->OnMoveItem(pAgpdItem, pAcUIGrid->m_lNowLayer, pAgpdGridSelectInfo->lGridRow, pAgpdGridSelectInfo->lGridColumn);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstSubNotify);

	return TRUE;
	}


BOOL AgcmUIRefinery::CBMoveEndItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl ||
		!pcsSourceControl->m_pcsBase ||
		AcUIBase::TYPE_GRID != pcsSourceControl->m_lType
		)
		return FALSE;

	AgcmUIRefinery		*pThis					= (AgcmUIRefinery *) pClass;
	AcUIGrid			*pAcUIGrid				= (AcUIGrid *) pcsSourceControl->m_pcsBase;
	AgpdGridSelectInfo	*pAgpdGridSelectInfo	= pAcUIGrid->GetGridItemClickInfo();

	AgpdGrid *pAgpdGrid = pThis->m_pAgcmUIManager2->GetControlGrid(pcsSourceControl);
	if (!pAgpdGridSelectInfo || !pAgpdGridSelectInfo->pGridItem || !pAgpdGrid)
		return FALSE;

	if (pThis->m_bHold)
		return FALSE;
	
	// remove from grid	
	pThis->m_pAgpmGrid->RemoveItem(pAgpdGrid, pAgpdGridSelectInfo->pGridItem);
	pThis->m_pAgpmGrid->Clear(pAgpdGrid, pAcUIGrid->m_lNowLayer, pAgpdGridSelectInfo->lGridRow, pAgpdGridSelectInfo->lGridColumn, 1, 1);
	pThis->RefreshSourceGrid();	

	// remove from list
	pThis->m_pSources[pAgpdGridSelectInfo->lGridRow * 5 + pAgpdGridSelectInfo->lGridColumn] = NULL;

    pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstSubNotify);
	//pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstActiveRefineButton); 항상 enable 로 바뀜.

	BOOL bEmpty = TRUE;
	for (INT16 i=0; i<AGPMREFINERY_MAX_GRID; ++i)
		{
		if (pThis->m_pSources[i])
			{
			bEmpty = FALSE;
			break;
			}
		}

	if (bEmpty)
		pThis->Reset();

	return TRUE;
	}


BOOL AgcmUIRefinery::CBOpenTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl ||
		pcsSourceControl->m_lType != AcUIBase::TYPE_GRID  || !pcsSourceControl->m_pcsBase )
		return FALSE;
	
	AgcmUIRefinery	*pThis = (AgcmUIRefinery *)		pClass;
	AcUIGrid		*pGrid = (AcUIGrid *)(pcsSourceControl->m_pcsBase);

	if (!pGrid->m_pToolTipAgpdGridItem || pGrid->m_pToolTipAgpdGridItem->m_eType != AGPDGRID_ITEM_TYPE_ITEM)
		return FALSE;
	
	if (!pThis->m_pAgcmUIItem)
		return FALSE;

	/*
	INT32 lIndex = pcsSourceControl->m_lUserDataIndex;
	AgpdGridItem *pAgpdGridItem = pThis->m_pAgpmGrid->GetItem(&pThis->m_AgpdGridSource, lIndex);
	if (!pAgpdGridItem)
		return FALSE;
	*/

	return pThis->m_pAgcmUIItem->OpenToolTip(pGrid->m_pToolTipAgpdGridItem->m_lItemID, pGrid->m_lItemToolTipX, pGrid->m_lItemToolTipY, FALSE, FALSE);
	}


BOOL AgcmUIRefinery::CBCloseTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIRefinery	*pThis = (AgcmUIRefinery *)		pClass;
	if (!pThis->m_pAgcmUIItem)
		return FALSE;

	return pThis->m_pAgcmUIItem->CloseToolTip();
	}




//	Display callbacks
//====================================================
//
BOOL AgcmUIRefinery::CBDisplayMainEdit(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || AGCDUI_USERDATA_TYPE_INT32 != eType || !pcsSourceControl)
		return FALSE;

	AgcmUIRefinery	*pThis = (AgcmUIRefinery *) pClass;
	
	if (AcUIBase::TYPE_EDIT != pcsSourceControl->m_lType || !pcsSourceControl->m_pcsBase)
		return FALSE;	

	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetLineDelimiter(pThis->m_pAgcmUIManager2->GetLineDelimiter());

	CHAR *psz = pThis->GetMessageTxt(pThis->m_eCategory, AGCMUIREFINERY_MESSAGE_MAIN_EDIT);

	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(psz);

	return TRUE;
	}	


BOOL AgcmUIRefinery::CBDisplaySubTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIRefinery	*pThis = (AgcmUIRefinery *) pClass;

	CHAR *psz = pThis->GetMessageTxt(pThis->m_eCategory, AGCMUIREFINERY_MESSAGE_SUB_TITLE);
	strcpy(szDisplay, psz ? psz : _T(""));

	return TRUE;
	}

BOOL AgcmUIRefinery::CBDisplaySubNotify(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || AGCDUI_USERDATA_TYPE_INT32 != eType || !pcsSourceControl)
		return FALSE;

	AgcmUIRefinery	*pThis = (AgcmUIRefinery *) pClass;

	if (AcUIBase::TYPE_EDIT != pcsSourceControl->m_lType || !pcsSourceControl->m_pcsBase)
		return FALSE;

	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetLineDelimiter(pThis->m_pAgcmUIManager2->GetLineDelimiter());

	CHAR *psz = NULL;
	
	psz = pThis->GetMessageTxt(pThis->m_eCategory, pThis->m_eNotifyMsg);
	CHAR sz[512];

	if (psz)
		{
		sprintf(sz, psz, pThis->m_pszNotify ? pThis->m_pszNotify : _T(""));
		((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(sz);
		}

	return TRUE;
	}

BOOL AgcmUIRefinery::CBDisplayPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || AGCDUI_USERDATA_TYPE_INT32 != eType)
		return FALSE;

	AgcmUIRefinery	*pThis = (AgcmUIRefinery *) pClass;
	
	CHAR szBuffer[32];
	strcpy(szBuffer, _T("0"));
	    
    if(pThis->m_pAgpmRefinery->m_pAgpdResultItem)
        sprintf(szBuffer, _T("%d"), pThis->m_pAgpmRefinery->m_pAgpdResultItem->m_lPrice);

    strcpy(szDisplay, szBuffer);

	return TRUE;
}

//	Boolean callback
//====================================================
//
//BOOL AgcmUIRefinery::CBIsActiveRefineButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
//	{
//	if (!pClass)
//		return FALSE;
//
//	AgcmUIRefinery *pThis	= (AgcmUIRefinery *)	pClass;
//
//	BOOL bResult = FALSE;
//	switch (pThis->m_eCategory)
//		{
//		case AGPMREFINERY_CATEGORY_STONE :
//		case AGPMREFINERY_CATEGORY_SKEL :
//			bResult = pThis->IsValidRefineStone();
//			break;
//		
//		case AGPMREFINERY_CATEGORY_ITEM :
//            bResult = TRUE;
//			break;
//		
//		case AGPMREFINERY_CATEGORY_KEY :
//		default:
//			break;
//		}	
//
//	return bResult;
//	}

//BOOL AgcmUIRefinery::IsValidRefineStone()
//	{
//	INT32 lResult = 0;
//	BOOL bResult = FALSE;
//	CHAR sz[512];
//	CHAR *psz = NULL;
//
//	if (NULL == m_pAgpdRefineTemplate)
//		return FALSE;
//	
//	if (m_pAgpmRefinery->IsValidStatus(m_pAgcmCharacter->GetSelfCharacter(), m_pAgpdRefineTemplate, &m_pSources[0], &lResult))
//		{
//		AgpdItemTemplate *pAgpdItemTemplate = m_pAgpmItem->GetItemTemplate(m_pAgpdRefineTemplate->m_lResultItemTID);
//		if (pAgpdItemTemplate)
//			{
//			m_eNotifyMsg = AGCMUIREFINERY_MESSAGE_REFINABLE;
//			m_pszNotify = pAgpdItemTemplate->m_szName;
//			psz = GetMessageTxt(m_eCategory, AGCMUIREFINERY_MESSAGE_REFINABLE);
//			if (psz)
//				sprintf(sz, psz, pAgpdItemTemplate->m_szName);
//			bResult = TRUE;
//			}
//		}
//	else
//		{
//		if (AGPMREFINERY_RESULT_FAIL_INSUFFICIENT_MONEY == lResult)
//			{
//			m_eNotifyMsg = AGCMUIREFINERY_MESSAGE_RESULT_FAIL_INSUFFICIENT_MONEY;
//			psz = GetMessageTxt(m_eCategory, m_eNotifyMsg);
//			if (psz)
//				strcpy(sz, psz);
//			}
//		else if (AGPMREFINERY_RESULT_FAIL_FULL_INVENTORY == lResult)
//			{
//			m_eNotifyMsg = AGCMUIREFINERY_MESSAGE_RESULT_FAIL_FULL_INVENTORY;
//			psz = GetMessageTxt(m_eCategory, m_eNotifyMsg);
//			if (psz)
//				strcpy(sz, psz);
//			}
//		}
//
//	m_pAgcmUIManager2->SetUserDataRefresh(m_pstSubNotify);
//	if (psz)
//		{
//		AddSystemMessage(sz);
//		}
//
//	return bResult;
//	}

//	Cooldown Callback
//======================================================
//
BOOL AgcmUIRefinery::CBAfterCooldown(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;
		
	AgcmUIRefinery *pThis = (AgcmUIRefinery *) pClass;
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return FALSE;
	
	INT32 lCID = pAgpdCharacter->m_lID;
	INT32 lID = 0;	
	
	//switch (pThis->m_eCategory)
	//{
	//	case AGPMREFINERY_CATEGORY_STONE :
	//	case AGPMREFINERY_CATEGORY_SKEL :
	//		{
	//		if (NULL == pThis->m_pAgpdRefineTemplate)
	//			return FALSE;
	//			
	//		lID = pThis->m_pAgpdRefineTemplate->m_lItemTID;
	//		}
	//		break;
	//	
	//	case AGPMREFINERY_CATEGORY_ITEM :
	//		break;
	//	
	//	case AGPMREFINERY_CATEGORY_KEY :
	//	default:
	//		return FALSE;
	//		break;
	//}	
	
	// source items	
	INT32 lItems[AGPMREFINERY_MAX_GRID];
	for (INT16 i = 0; i<AGPMREFINERY_MAX_GRID; ++i)
	{
		if (pThis->m_pSources[i])
			lItems[i] = pThis->m_pSources[i]->m_lID;
		else
			lItems[i] = 0;

    }
	
	//if (AGPMREFINERY_CATEGORY_ITEM == pThis->m_eCategory)
	    return pThis->m_pAgcmRefinery->SendPacketRefineItem(lCID, lID, lItems);	
	//else
	//	return pThis->m_pAgcmRefinery->SendPacketRefine(lCID, lID, lItems);	
}




//	Result callback
//=======================================================
//
BOOL AgcmUIRefinery::CBRefineResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUIRefinery *pThis = (AgcmUIRefinery *) pClass;

	INT32 lResult = (INT32) pCustData;
	INT32 lResultItemTID = (INT32) pData;

	pThis->OnResult(lResult, lResultItemTID);

	return TRUE;
	}




//	Grant callback
//==========================================================
//
BOOL AgcmUIRefinery::CBGrant(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIRefinery	*pThis = (AgcmUIRefinery *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;
					
	pThis->m_stOpenPos	= pAgpdCharacter->m_stPos;
	pThis->Open();

	return TRUE;
}


BOOL AgcmUIRefinery::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIRefinery	*pThis = (AgcmUIRefinery *) pClass;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pData;

	if (!pThis->m_bIsOpen)
		return TRUE;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pAgpdCharacter->m_stPos, pThis->m_stOpenPos);
	if ((INT32) fDistance < 150)
		return TRUE;

	pThis->Reset(FALSE);
	pThis->m_bIsOpen = FALSE;
	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIREFINERY_EVENT_CLOSE]);

	return TRUE;
	}




//	System driven UI close callback
//==================================================
//
BOOL AgcmUIRefinery::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUIRefinery *pThis = (AgcmUIRefinery *) pClass;
	
	pThis->Reset(FALSE);

	return TRUE;
	}


BOOL AgcmUIRefinery::CBCloseAllUIToolTip(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUIRefinery *pThis = (AgcmUIRefinery *) pClass;
	pThis->m_csToolTip.DeleteAllStringInfo();

	return TRUE;
	}




//	General helper
//=======================================================
//
void AgcmUIRefinery::Reset(BOOL bRefresh)
	{
	m_eResult = AGPMREFINERY_RESULT_NONE;
	m_pAgpdRefineTemplate = NULL;
	m_bHold = FALSE;
	m_eNotifyMsg = AGCMUIREFINERY_MESSAGE_SUB_NOTIFY;
	m_pszNotify = NULL;
	m_pSources.MemSetAll();

	ClearSourceGrid();
	ClearResultGrid();
	
	// reset message
	if (bRefresh)
		{
		m_pAgcmUIManager2->SetUserDataRefresh(m_pstSubTitle);
		m_pAgcmUIManager2->SetUserDataRefresh(m_pstSubNotify);
		m_pAgcmUIManager2->SetUserDataRefresh(m_pstPrice);
		m_pAgcmUIManager2->SetUserDataRefresh(m_pstActiveRefineButton);
		
		m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIREFINERY_EVENT_RESET]);
		}
	}


CHAR* AgcmUIRefinery::GetMessageTxt(INT32 eCategory, eAGCMUIREFINERY_MESSAGE eMessage)
	{
	CHAR szKey[32];
	CHAR szPost[20];

	if (AGCMUIREFINERY_MESSAGE_MAIN_EDIT == eMessage)
		{
		strcpy(szKey, s_szMessage[AGCMUIREFINERY_MESSAGE_MAIN_EDIT]);
		//return m_pAgcmUIManager2->GetUIMessage(szKey);
		//return s_szMain;
		return ClientStr().GetStr(STI_REFINERY_INTRO);
		}

	switch (eCategory)
		{
		case AGPMREFINERY_CATEGORY_STONE :		
			strcpy(szPost, _T("_STONE"));
			break;
					
		case AGPMREFINERY_CATEGORY_SKEL :
			strcpy(szPost, _T("_SKEL"));
			break;
			
		case AGPMREFINERY_CATEGORY_ITEM:
			strcpy(szPost, _T("_ITEM"));
			break;
			
		default :
			strcpy(szPost, _T(""));
			break;
		}


	sprintf(szKey, _T("%s%s"), s_szMessage[eMessage], szPost);

	return m_pAgcmUIManager2->GetUIMessage(szKey);
	}


BOOL AgcmUIRefinery::AddSystemMessage(CHAR* pszMsg)
	{
	if(!pszMsg)
		return FALSE;

	AgpdChatData	stChatData;
	memset(&stChatData, 0, sizeof(stChatData));

	stChatData.eChatType = AGPDCHATTING_TYPE_SYSTEM_LEVEL1;
	stChatData.szMessage = pszMsg;

	stChatData.lMessageLength = strlen(stChatData.szMessage);
		
	m_pAgcmChatting->AddChatMessage(AGCMCHATTING_TYPE_SYSTEM, &stChatData);
	m_pAgcmChatting->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);

	return TRUE;
	}




//	OnAddModule helper
//================================================
//
BOOL AgcmUIRefinery::AddEvent()
	{
	for (int i=0; i < AGCMUIREFINERY_EVENT_END; ++i)
	{
		m_lEvent[i] = m_pAgcmUIManager2->AddEvent(s_szEvent[i]);
		if (m_lEvent[i] < 0)
			return FALSE;
	}
    m_lEvent[AGCMUIREFINERY_EVENT_REFINEMONEYMSGBOX_OPEN] =
        m_pAgcmUIManager2->AddEvent(s_szEvent[AGCMUIREFINERY_EVENT_REFINEMONEYMSGBOX_OPEN], CBRefine, this);

	return TRUE;	
	}


BOOL AgcmUIRefinery::AddFunction()
	{
	if (!m_pAgcmUIManager2->AddFunction(this, "REFINE_OPEN", CBOpenWindow, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "REFINE_CLOSE", CBCloseWindow, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "REFINE_TOOLTIP_OPEN", CBOpenTooltip, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "REFINE_TOOLTIP_CLOSE", CBCloseTooltip, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "REFINE_MOVE_ITEM", CBMoveItem, 0))
		return FALSE;	
	if (!m_pAgcmUIManager2->AddFunction(this, "REFINE_MOVEEND_ITEM", CBMoveEndItem, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "REFINE_REFINEBTN_CLICK", CBClickRefineButton, 0))
		return FALSE;
    if (!m_pAgcmUIManager2->AddFunction(this, "REFINE_RESETBTN_CLICK", CBClickResetButton, 0))
		return FALSE;
	return TRUE;
	}


BOOL AgcmUIRefinery::AddDisplay()
	{
	if (!m_pAgcmUIManager2->AddDisplay(this, "REFINE_DISP_MAIN_EDIT", 0, CBDisplayMainEdit, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "REFINE_DISP_SUB_TITLE", 0, CBDisplaySubTitle, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "REFINE_DISP_SUB_NOTIFY", 0, CBDisplaySubNotify, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "REFINE_DISP_PRICE", 0, CBDisplayPrice, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIRefinery::AddUserData()
	{
	// active tab
	m_pstActiveTab = m_pAgcmUIManager2->AddUserData("REFINE_ACTIVE_TAB", &m_lDummy, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstActiveTab)
		return FALSE;
		
	// main edit
	m_pstMainEdit = m_pAgcmUIManager2->AddUserData("REFINE_UD_MAIN_EDIT", NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstMainEdit)
		return FALSE;

	// sub title
	m_pstSubTitle = m_pAgcmUIManager2->AddUserData("REFINE_UD_SUB_TITLE", NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstSubTitle)
		return FALSE;

	// sub notify
	m_pstSubNotify = m_pAgcmUIManager2->AddUserData("REFINE_UD_SUB_NOTIFY", NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstSubNotify)
		return FALSE;

	// price
	m_pstPrice = m_pAgcmUIManager2->AddUserData("REFINE_UD_PRICE", NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstPrice)
		return FALSE;

	// source item grid
	m_pstSourceItem = m_pAgcmUIManager2->AddUserData("REFINE_UD_SOURCE_GRID", &m_AgpdGridSource, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
	if (NULL == m_pstSourceItem)
		return FALSE;

	// result item grid
	m_pstResultItem = m_pAgcmUIManager2->AddUserData("REFINE_UD_RESULT_GRID", &m_AgpdGridResult, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
	if (NULL == m_pstResultItem)
		return FALSE;

	m_pstActiveRefineButton = m_pAgcmUIManager2->AddUserData("REFINE_UD_REFINE_BTN", &m_lRefineButton, sizeof(INT32),
														  1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstActiveRefineButton)
		return FALSE;

	return TRUE;
	}


//BOOL AgcmUIRefinery::AddBoolean()
//{
//	//if (!m_pAgcmUIManager2->AddBoolean(this, "REFINE_ACTIVE_REFINE_BTN", CBIsActiveRefineButton, AGCDUI_USERDATA_TYPE_INT32))
//	//	return FALSE;
//
//	return TRUE;
//}

//	Grid display helper
//==========================================================
BOOL AgcmUIRefinery::InitializeGrid()
{
	// grid
	m_pAgpmGrid->Init(&m_AgpdGridSource, 1, 2, 5);
	m_AgpdGridSource.m_lGridType = AGPDGRID_ITEM_TYPE_ITEM;
	
	m_pAgpmGrid->Init(&m_AgpdGridResult, 1, 1, 1);
	m_AgpdGridResult.m_lGridType = AGPDGRID_ITEM_TYPE_ITEM;
	
	// grid item
	m_pAgpdGridItemResult = m_pAgpmGrid->CreateGridItem();
	if (!m_pAgpdGridItemResult)
		return FALSE;
		
	return TRUE;
}


void AgcmUIRefinery::DestroyGrid()
{
	// grid item
	if (m_pAgpdGridItemResult)
		{
		m_pAgpmGrid->DeleteGridItem(m_pAgpdGridItemResult);
		m_pAgpdGridItemResult = NULL;
		}
	
	// grid	
	m_pAgpmGrid->Remove(&m_AgpdGridSource);
	m_pAgpmGrid->Remove(&m_AgpdGridResult);
}


BOOL AgcmUIRefinery::AddItemToSourceGrid(AgpdItem *pAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn)
	{
	m_pAgpmGrid->AddItem(&m_AgpdGridSource, pAgpdItem->m_pcsGridItem);
	return m_pAgpmGrid->Add(&m_AgpdGridSource, nLayer, nRow, nColumn, pAgpdItem->m_pcsGridItem, 1, 1);
	}


BOOL AgcmUIRefinery::RemoveItemFromSourceGrid(AgpdItem *pAgpdItem)
	{
	return m_pAgpmGrid->RemoveItem(&m_AgpdGridSource, pAgpdItem->m_pcsGridItem);
	}


BOOL AgcmUIRefinery::AddItemToResultGrid(AgpdItemTemplate *pAgpdItemTemplate, INT32 lCount)
	{
	SetGridItemAttachedTexture(m_pAgpdGridItemResult, pAgpdItemTemplate);
	
	// ################
	//if (pAgpdItemTemplate->m_bStackable)
		{
		CHAR sz[AGPDGRIDITEM_BOTTOM_STRING_LENGTH + 1] = {0,};
		sprintf(sz, "%d", lCount);
		m_pAgpdGridItemResult->SetRightBottomString(sz);
		}

	m_pAgpmGrid->AddItem(&m_AgpdGridResult, m_pAgpdGridItemResult);
	m_pAgpmGrid->Add(&m_AgpdGridResult, m_pAgpdGridItemResult, 1, 1);

	return TRUE;
	}


void AgcmUIRefinery::RefreshSourceGrid()
	{
	if (m_pstSourceItem)
		{
		m_pstSourceItem->m_stUserData.m_pvData	= &m_AgpdGridSource;
		m_pAgcmUIManager2->SetUserDataRefresh(m_pstSourceItem);
		}
	}
	

void AgcmUIRefinery::RefreshResultGrid()
	{
	if (m_pstResultItem)
		{
		m_pstResultItem->m_stUserData.m_pvData	= &m_AgpdGridResult;
		m_pAgcmUIManager2->SetUserDataRefresh(m_pstResultItem);
		}
	}


BOOL AgcmUIRefinery::SetGridItemAttachedTexture(AgpdGridItem *pAgpdGridItem, AgpdItemTemplate *pAgpdItemTempalte)
	{
	if (!pAgpdGridItem)
		return FALSE;

	RwTexture **ppRwTexture = (RwTexture **)(m_pAgpmGrid->GetAttachedModuleData( 
			m_pAgcmUIControl->m_lItemGridTextureADDataIndex, pAgpdGridItem ));

	AgcdItemTemplate *pAgcdItemTemplate = m_pAgcmItem->GetTemplateData(pAgpdItemTempalte);
		
	(RwTexture *) *ppRwTexture = pAgcdItemTemplate->m_pTexture;

	return TRUE;		
	}


void AgcmUIRefinery::ClearSourceGrid()
	{
	m_pAgpmGrid->Reset(&m_AgpdGridSource);
	RefreshSourceGrid();
	}


void AgcmUIRefinery::ClearResultGrid()
	{	
	m_pAgpmGrid->Reset(&m_AgpdGridResult);
	RefreshResultGrid();
	}


BOOL AgcmUIRefinery::CBRemoveItemFromInventory(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIRefinery	*pThis	= (AgcmUIRefinery *)	pClass;
	AgpdItem		*pcsItem	= (AgpdItem *)	pData;

	pThis->m_pAgpmGrid->RemoveItem(&pThis->m_AgpdGridSource, pcsItem->m_pcsGridItem);

	pThis->RefreshSourceGrid();

	return TRUE;
	}

BOOL AgcmUIRefinery::CBCheckUseItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgcmUIRefinery* pThis = static_cast<AgcmUIRefinery*>(pClass);
	AgpdItem* pcsItem = static_cast<AgpdItem*>(pData);

	// If the item exists in sources, return false.
	for(int i = 0; i < AGPMREFINERY_MAX_GRID; ++i)
	{
		if(pThis->m_pSources[i] && pThis->m_pSources[i]->m_lID == pcsItem->m_lID)
			return FALSE;
	}

	return TRUE;
}