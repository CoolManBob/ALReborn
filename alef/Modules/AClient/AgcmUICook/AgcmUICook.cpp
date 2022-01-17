/*==============================================================

	AgcmUICook.cpp

==============================================================*/

#include "AgcmUICook.h"
#include "AgpdProduct.h"

CHAR g_szEvent[AGCMUICOOK_EVENT_MAX][30] = 
	{
	"UI_COOK_SELECT_OPEN",
	"UI_COOK_SELECT_CLOSE",
	"UI_COOK_SELECT_LIST",
	"UI_COOK_MAKE_OPEN",
	"UI_COOK_MAKE_CLOSE",
	"UI_COOK_MAKE_SUCCESS",
	"UI_COOK_MAKE_FAIL",
	"UI_GATHER_SUCCESS",
	"UI_GATHER_FAIL"
	};

/****************************************************/
/*		The Implementation of AgcmUICook class		*/
/****************************************************/
//
AgcmUICook::AgcmUICook()
	{
	SetModuleName("AgcmUICook");

	m_pcsAgpmCharacter	= NULL;
	m_pcsAgcmCharacter	= NULL;
	m_pcsAgcmUIManager2	= NULL;
	m_pcsAgpmProduct	= NULL;
	m_pcsAgpmGrid		= NULL;
	m_pcsAgcmItem		= NULL;
	m_pcsAgcmUIControl	= NULL;
	m_pcsAgpmItem		= NULL;
	m_pcsAgpmFactors	= NULL;
	m_pcsAgcmProduct	= NULL;
	m_pcsAgpmSkill		= NULL;
	m_pcsAgcmSkill		= NULL;
	m_pcsAgcmChatting	= NULL;

	// user data
	m_pstSelectList			= NULL;
	m_pstMakeEdit			= NULL;
	m_pstMakeSourceItem		= NULL;
	m_pstMakeOk				= NULL;
	m_pstMakeCancel			= NULL;
	
	ZeroMemory(m_lCookList, sizeof(INT32) * AGCMUICOOK_MAX_LIST);
	ZeroMemory(m_pGridItemList, sizeof(AgpdGridItem*) * AGCMUICOOK_MAX_GRID);

	m_lSelectedCookID		= 0;
	m_lReceipeID			= 0;
	m_lCookIndex			= 0;
	m_lCookCount			= 0;

	m_bHoldOn				= FALSE;

	ZeroMemory(&m_stOpenPos, sizeof(AuPOS));

	m_bIsWindowOpen			= FALSE;
	}

AgcmUICook::~AgcmUICook()
	{
	}


//	ApModule inherited
//===============================================
//
BOOL AgcmUICook::OnInit()
	{
	m_csToolTip.m_Property.bTopmost = TRUE;
	m_pcsAgcmUIManager2->AddWindow((AgcWindow *) (&m_csToolTip));
	m_csToolTip.ShowWindow(FALSE);
	
	m_pcsAgpmGrid->Init(&m_GridSourceItem, 1, 2, 4);
	m_GridSourceItem.m_lGridType	= AGPDGRID_ITEM_TYPE_ITEM;
	
	return TRUE;
	}

BOOL AgcmUICook::OnDestroy()
	{
	ClearGrid();
	m_pcsAgpmGrid->Remove(&m_GridSourceItem);
	return TRUE;
	}

BOOL AgcmUICook::OnAddModule()
	{
	m_pcsAgpmCharacter	= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgcmCharacter	= (AgcmCharacter*)GetModule("AgcmCharacter");
	m_pcsAgpmProduct	= (AgpmProduct*)GetModule("AgpmProduct");
	m_pcsAgcmUIManager2 = (AgcmUIManager2*)GetModule("AgcmUIManager2");
	m_pcsAgpmGrid		= (AgpmGrid*)GetModule("AgpmGrid");
	m_pcsAgpmItem		= (AgpmItem*)GetModule("AgpmItem");
	m_pcsAgcmItem		= (AgcmItem*)GetModule("AgcmItem");
	m_pcsAgcmUIControl	= (AgcmUIControl*)GetModule("AgcmUIControl");
	m_pcsAgpmFactors	= (AgpmFactors*)GetModule("AgpmFactors");
	m_pcsAgcmProduct	= (AgcmProduct*)GetModule("AgcmProduct");
	m_pcsAgpmSkill		= (AgpmSkill*)GetModule("AgpmSkill");
	m_pcsAgcmSkill		= (AgcmSkill*)GetModule("AgcmSkill");
	m_pcsAgcmChatting	= (AgcmChatting2*)GetModule("AgcmChatting2");
	m_pcsAgcmUIMain		= (AgcmUIMain*)GetModule("AgcmUIMain");

	if (!m_pcsAgpmCharacter ||!m_pcsAgcmCharacter || !m_pcsAgpmProduct ||
		!m_pcsAgcmUIManager2 || !m_pcsAgpmGrid || 
		!m_pcsAgcmItem || !m_pcsAgcmUIControl || !m_pcsAgpmItem || !m_pcsAgpmFactors ||
		!m_pcsAgcmProduct || !m_pcsAgpmSkill || !m_pcsAgcmChatting)
		return FALSE;
	
	// make result callback
	if (!m_pcsAgpmProduct->SetCBProductResult(CBProductResult, this))
		return FALSE;

	if (m_pcsAgcmSkill && !m_pcsAgcmSkill->SetCallbackPreProcessSkill(CBCastCook, this))
		return FALSE;

	if (m_pcsAgcmUIMain && !m_pcsAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	if (m_pcsAgcmUIMain && !m_pcsAgcmUIMain->SetCallbackCloseAllUITooltip(CBCloseAllUIToolTip, this))
		return FALSE;


	if (!AddEvent() ||
		!AddFunction() ||
		!AddDisplay() ||
		!AddUserData()
		)
		return FALSE;

	return TRUE;
}


BOOL AgcmUICook::AddEvent()
	{
	for (int i=0; i < AGCMUICOOK_EVENT_MAX; ++i)
		{
		m_lEvent[i] = m_pcsAgcmUIManager2->AddEvent(g_szEvent[i]);
		if (m_lEvent[i] < 0)
			return FALSE;
		}

	return TRUE;	
	}

BOOL AgcmUICook::AddFunction()
	{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "COOK_SELECT_WINDOW_OPEN", CBOpenSelectWindow, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "COOK_SELECT_LIST_ITEM_CLICK", CBClickSelectListItem, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "COOK_SELECT_TOOLTIP_OPEN", CBOpenSelectTooltip, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "COOK_SELECT_TOOLTIP_CLOSE", CBCloseSelectTooltip, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "COOK_MAKE_WINDOW_OPEN", CBOpenMakeWindow, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "COOK_MAKE_OKBUTTON_CLICK", CBClickMakeOkButton, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "COOK_MAKE_CANCELBUTTON_CLICK", CBClickMakeCancelButton, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "COOK_WINDOW_CLOSE", CBCloseWindow, 0))
		return FALSE;

	return TRUE;
	}


BOOL AgcmUICook::AddDisplay()
	{
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Cook_Display_Select_List", 0, CBDisplaySelectList, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Cook_Display_Make_Edit", 0, CBDisplayMakeEdit, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Cook_Display_Make_Ok", 0, CBDisplayMakeOk, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Cook_Display_Make_Cancel", 0, CBDisplayMakeCancel, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
	}

BOOL AgcmUICook::AddUserData()
	{
	m_pstSelectList = m_pcsAgcmUIManager2->AddUserData("Cook_UD_Select_List", m_lCookList, sizeof(INT32),
													 AGCMUICOOK_MAX_PAGE_LIST, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstSelectList)
		return FALSE;

	m_pstMakeEdit = m_pcsAgcmUIManager2->AddUserData("Cook_UD_Make_Edit", NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstMakeEdit)
		return FALSE;

	m_pstMakeSourceItem = m_pcsAgcmUIManager2->AddUserData("Cook_UD_Make_ItemGrid", &m_GridSourceItem,
														   sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
	if (NULL == m_pstMakeSourceItem)
		return FALSE;

	m_pstMakeOk = m_pcsAgcmUIManager2->AddUserData("Cook_UD_Make_Ok", NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstMakeOk)
		return FALSE;

	m_pstMakeCancel = m_pcsAgcmUIManager2->AddUserData("Cook_UD_Make_Cancel", NULL, 0,
														 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstMakeCancel)
		return FALSE;

	return TRUE;
	}

//	Functions
//=========================================================
//
BOOL AgcmUICook::CBOpenSelectWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	AgcmUICook		*pThis = (AgcmUICook *) pClass;
	AgcdUIControl	*pcsControl = (AgcdUIControl *) pData1;
	BOOL			bResult = FALSE;

	pThis->m_lSelectedCookID = 0;
	pThis->ClearGrid();

	if (NULL != pcsControl)
		{
		if (AcUIBase::TYPE_EDIT != pcsControl->m_lType || !pcsControl->m_pcsBase)
			return FALSE;

		((AcUIEdit *)pcsControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());

		// ASSERT category is COOK
		bResult = ((AcUIEdit *)pcsControl->m_pcsBase)->SetText(
						pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_SELECT));
		}

	pThis->m_pstSelectList->m_stUserData.m_lCount = pThis->m_lCookCount;
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstSelectList);

	return bResult;
	}

BOOL AgcmUICook::CBClickSelectListItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICook	*pThis = (AgcmUICook*)pClass;
	AgpdProduct* pProduct = pThis->GetProductByIndex(pcsSourceControl->m_lUserDataIndex);
	if (!pProduct)
		return FALSE;

	pThis->m_lSelectedCookID = pProduct->m_lProductID;
	pThis->m_lReceipeID = 0;

	if (pThis->m_pcsAgpmProduct->IsValidStatus(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pProduct))
		{
		pThis->m_csToolTip.ShowWindow(FALSE);
		pThis->m_csToolTip.DeleteAllStringInfo();

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_SELECT_CLOSE]);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_MAKE_OPEN]);
		}

	// receipe check
	if (pThis->m_pcsAgpmProduct->IsValidItemStatus(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pProduct,
													pProduct->m_csReceipe.m_pcsItemTemplate))
	{
		pThis->m_lReceipeID = pProduct->m_csReceipe.m_pcsItemTemplate->m_lID;
	}

	return TRUE;
	}

BOOL AgcmUICook::CBOpenSelectTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICook	*pThis = (AgcmUICook *)	pClass;
	INT32		lX	=	0;
	INT32		lY	=	0;

	pcsSourceControl->m_pcsBase->ClientToScreen(&lX, &lY);

	lX += pcsSourceControl->m_pcsBase->w;
	lY += pcsSourceControl->m_pcsBase->h;

	pThis->m_csToolTip.MoveWindow(lX, lY, pThis->m_csToolTip.w, pThis->m_csToolTip.h);

	AgpdProduct* pProduct = pThis->GetProductByIndex(pcsSourceControl->m_lUserDataIndex);
	if (!pProduct)
		return FALSE;

	// ASSERT category == COOK
	pThis->SetTooltipText(pProduct);
	pThis->m_csToolTip.ShowWindow(TRUE);

	return TRUE;
	}

BOOL AgcmUICook::CBCloseSelectTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICook	*pThis = (AgcmUICook *)	pClass;

	pThis->m_csToolTip.ShowWindow(FALSE);
	pThis->m_csToolTip.DeleteAllStringInfo();

	return TRUE;
	}

BOOL AgcmUICook::CBOpenMakeWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	AgcmUICook *pThis = (AgcmUICook*)pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;
	BOOL bResult = FALSE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstMakeOk);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstMakeCancel);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstMakeEdit);
	pThis->m_bHoldOn = FALSE;

	return pThis->DisplaySourceItem();
	}

BOOL AgcmUICook::CBClickMakeOkButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICook	*pThis = (AgcmUICook *)	pClass;
	
	if (pThis->m_bHoldOn)
		return FALSE;

	pThis->m_bHoldOn = TRUE;
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_MAKE_SUCCESS]);

	if (pThis->m_pcsAgcmProduct)
		pThis->m_pcsAgcmProduct->SendPacketProduct(pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_lID, 
												   pThis->m_lSelectedCookID,
												   pThis->m_lReceipeID);
	return TRUE;	
	}

BOOL AgcmUICook::CBClickMakeCancelButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICook	*pThis = (AgcmUICook *)	pClass;
	
	// 아이템 조합 리스트로 돌아간다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_MAKE_CLOSE]);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_SELECT_OPEN]);
	
	return TRUE;	
	}

BOOL AgcmUICook::CBCloseWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICook	*pThis = (AgcmUICook *)	pClass;
	
	pThis->m_bIsWindowOpen = FALSE;
	pThis->m_bHoldOn = FALSE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_MAKE_CLOSE]);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_SELECT_CLOSE]);
	
	return TRUE;	
	}

//	Display callbacks
//====================================================
//
BOOL AgcmUICook::CBDisplaySelectList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	// 결과물의 이름을 List에 보여준다.
	if (!pData || !pClass || AGCDUI_USERDATA_TYPE_INT32 != eType)
		return FALSE;

	AgcmUICook *pThis = (AgcmUICook *) pClass;
	AgpdProduct *pProduct = pThis->GetProductByIndex(pcsSourceControl->m_lUserDataIndex);

	if (pProduct)
		{
		sprintf(szDisplay, "%s", pProduct->m_csResultItem.m_pcsItemTemplate->m_szName);

		// 제조 가능한 상태인지 아닌지에 따라 글자 색상을 다르게 준다. A/B/G/R <- Color key
		if (pThis->m_pcsAgpmProduct->IsValidStatus(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pProduct))
			pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor = COLOR_WHITE;
		else
			pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor = COLOR_GRAY;
		}
	else
		szDisplay[0] = NULL;
	
	return TRUE;	
	}

BOOL AgcmUICook::CBDisplayMakeEdit(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || AGCDUI_USERDATA_TYPE_INT32 != eType)
		return FALSE;

	AgcmUICook		*pThis = (AgcmUICook *) pClass;

	if( !pcsSourceControl )
		return FALSE;
	if (AcUIBase::TYPE_EDIT != pcsSourceControl->m_lType || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AgpdProduct *pProduct = pThis->m_pcsAgpmProduct->GetProductData(pThis->m_lSelectedCookID);
	if (!pProduct)
		return FALSE;

	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());

	CHAR szBuffer[512] = {0,};
	
	// ASSERT category == COOK
	strncpy(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_MAKE), 128);
	sprintf(szBuffer + strlen(szBuffer), "______"
										"[%s]___" 
										"- 요구 재료 -___",
										pProduct->m_csResultItem.m_pcsItemTemplate->m_szName);

	for (int i = 0; i < pProduct->m_lRequireItemCount; ++i)
		{
		sprintf(szBuffer + strlen(szBuffer), "%s %d개___", pProduct->m_csRequireItemList[i].m_pcsItemTemplate->m_szName,
															pProduct->m_csRequireItemList[i].m_lCount);
		}

	if (pProduct->m_csReceipe.m_pcsItemTemplate)
		sprintf(szBuffer + strlen(szBuffer), "%s[옵션]___",
										pProduct->m_csReceipe.m_pcsItemTemplate->m_szName);

	sprintf(szBuffer + strlen(szBuffer), "요리 스킬 LV %d이상______",
										pProduct->m_lItemLevel);

	strncpy(szBuffer + strlen(szBuffer), pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_MAKE2), 128);

	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(szBuffer);
	return TRUE;
	}

BOOL AgcmUICook::CBDisplayMakeOk(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUICook	*pThis = (AgcmUICook *) pClass;

	sprintf(szDisplay, "%s", "요리 만들기");

	return TRUE;	
	}

BOOL AgcmUICook::CBDisplayMakeCancel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUICook	*pThis = (AgcmUICook *) pClass;

	sprintf(szDisplay, "%s", "취소");

	return TRUE;	
	}


//	Display helper
//==========================================================
//
BOOL AgcmUICook::DisplaySourceItem()
	{
	AgpdProduct* pProduct = m_pcsAgpmProduct->GetProductData(m_lSelectedCookID);
	if (!pProduct) return FALSE;

	m_lCookIndex = 0;

	for (int i = 0; i < pProduct->m_lRequireItemCount; ++i)
		{
		AddGridItem(pProduct->m_csRequireItemList[i].m_pcsItemTemplate, pProduct->m_csRequireItemList[i].m_lCount);
		}

	if (0 != m_lReceipeID)
		{
		AddGridItem(pProduct->m_csReceipe.m_pcsItemTemplate, 1);
		}

	RefreshSourceItemGrid();
	return TRUE;	
	}

BOOL AgcmUICook::RefreshSourceItemGrid()
	{
	if (m_pstMakeSourceItem)
		{
		m_pstMakeSourceItem->m_stUserData.m_pvData	= &m_GridSourceItem;
		return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstMakeSourceItem);
		}

	return TRUE;
	}

BOOL AgcmUICook::ClearGrid()
	{
	m_pcsAgpmGrid->Reset(&m_GridSourceItem);
	RefreshSourceItemGrid();

	//m_pcsAgpmGrid->Reset(&m_GridReceipe);
	//RefreshReceipe();
	
	for (int i = 0; i < AGCMUICOOK_MAX_GRID; ++i)
		{
		if (!m_pGridItemList[i]) 
			break;

		m_pcsAgpmGrid->DeleteGridItem(m_pGridItemList[i]);
		}

	ZeroMemory(m_pGridItemList, sizeof(AgpdGridItem*) * AGCMUICOOK_MAX_GRID);

	return TRUE;
	}

BOOL AgcmUICook::SetGridItemAttachedTexture( AgpdGridItem* pcsAgpdGridItem, AgpdItemTemplate* pcsAgpdItemTempalte )
	{
	if (!pcsAgpdGridItem)
		return FALSE;

	RwTexture** ppRwTexture = (RwTexture**)(m_pcsAgpmGrid->GetAttachedModuleData( 
			m_pcsAgcmUIControl->m_lItemGridTextureADDataIndex, pcsAgpdGridItem ));

	AgcdItemTemplate* pcsAgcdItemTemplate = m_pcsAgcmItem->GetTemplateData(pcsAgpdItemTempalte);
		
	(RwTexture*) *ppRwTexture = pcsAgcdItemTemplate->m_pTexture;

	return TRUE;		
	}

BOOL AgcmUICook::SetTooltipText(AgpdProduct* pProduct)
	{
	CHAR	szBuffer[128];
	ZeroMemory(szBuffer, sizeof(CHAR) * 128);

	AgpdCharacter* pAgpdCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pAgpdCharacter)
		return FALSE;

	// 결과 아이템
	sprintf(szBuffer, "[%s]", pProduct->m_csResultItem.m_pcsItemTemplate->m_szName);
	m_csToolTip.AddString(szBuffer, 14, COLOR_WHITE);
	m_csToolTip.AddNewLine(14);

	// 재료 아이템 정보 구성
	for (int i = 0; i < pProduct->m_lRequireItemCount; ++i)
		{
		sprintf(szBuffer, "%s %d개", pProduct->m_csRequireItemList[i].m_pcsItemTemplate->m_szName, 
									pProduct->m_csRequireItemList[i].m_lCount);
		if (m_pcsAgpmProduct->IsValidItemStatus(m_pcsAgcmCharacter->GetSelfCharacter(), pProduct, 
												pProduct->m_csRequireItemList[i].m_pcsItemTemplate, 
												pProduct->m_csRequireItemList[i].m_lCount))
			m_csToolTip.AddString(szBuffer, 14, COLOR_WHITE);
		else
			m_csToolTip.AddString(szBuffer, 14, COLOR_RED);
		m_csToolTip.AddNewLine(14);
		}

	m_csToolTip.AddNewLine(14);

	// 레시피 정보
	if (pProduct->m_csReceipe.m_pcsItemTemplate)
	{
		sprintf(szBuffer, "[옵션]");
		m_csToolTip.AddString(szBuffer, 14, COLOR_WHITE);
		m_csToolTip.AddNewLine(14);
		
		sprintf(szBuffer, "%s", pProduct->m_csReceipe.m_pcsItemTemplate->m_szName);
		if (m_pcsAgpmProduct->IsValidItemStatus(m_pcsAgcmCharacter->GetSelfCharacter(), pProduct, 
											pProduct->m_csReceipe.m_pcsItemTemplate, pProduct->m_csReceipe.m_lCount))
			m_csToolTip.AddString(szBuffer, 14, COLOR_WHITE);
		else
			m_csToolTip.AddString(szBuffer, 14, COLOR_RED);
		m_csToolTip.AddNewLine(14);
		m_csToolTip.AddNewLine(14);
	}

	// 조합비 정보 구성
	/*
	sprintf(szBuffer, "조합비 %d겔드 소요", pProduct->m_lMakingPrice);
	if (pAgpdCharacter->m_llMoney >= pProduct->m_lMakingPrice)
		m_csToolTip.AddString(szBuffer, 14, COLOR_WHITE);
	else
		m_csToolTip.AddString(szBuffer, 14, COLOR_RED);
	m_csToolTip.AddNewLine(14);	
	*/

	return TRUE;
	}


//	Helper
//=======================================================
//
BOOL AgcmUICook::AddGridItem(AgpdItemTemplate* pItemTemplate, INT32 lCount)
	{
	m_pGridItemList[m_lCookIndex] = m_pcsAgpmGrid->CreateGridItem();
	
	SetGridItemAttachedTexture(m_pGridItemList[m_lCookIndex], pItemTemplate);
	
	if (pItemTemplate->m_bStackable)
		{
		CHAR szRightString[AGPDGRIDITEM_BOTTOM_STRING_LENGTH + 1] = {0,};
		sprintf(szRightString, "%d", lCount);
		m_pGridItemList[m_lCookIndex]->SetRightBottomString(szRightString);
		}

	m_pcsAgpmGrid->AddItem(&m_GridSourceItem, m_pGridItemList[m_lCookIndex]);
	m_pcsAgpmGrid->Add(&m_GridSourceItem, m_pGridItemList[m_lCookIndex], 1, 1);

	++m_lCookIndex;

	return TRUE;
	}

AgpdProduct* AgcmUICook::GetProductByIndex(INT32 lUserDataIndex)
	{
	INT32 lIndex = lUserDataIndex;
	return m_pcsAgpmProduct->GetProductData(m_lCookList[lIndex]);
	}


//	Grant callback
//==========================================================
//
BOOL AgcmUICook::CBCastCook(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICook *pThis = (AgcmUICook *) pClass;
					
	INT32 lSkillID		= (INT32) ((PVOID *)pData)[0];

	//INT32 lTargetCID	= (INT32) ((PVOID *)pData)[1];
	//BOOL bForceAttack = (BOOL) ((PVOID *)pData)[2];
	BOOL *pbProcessed	= (BOOL *) pCustData;

	AgpdSkill *pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(lSkillID);
	if (!pcsSkill || !pcsSkill->m_pcsTemplate)
		return TRUE;

	AgpdSkillTemplate *pcsAgpdSkillTemplate = (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;

	// hard-coded
	if (0 == strcmp(pcsAgpdSkillTemplate->m_szName, "요리하기"))
	{
		*pbProcessed = TRUE;
		if (pThis->m_bIsWindowOpen)
			return TRUE;

		pThis->m_lCookCount = 0;
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_SELECT_OPEN]);

		ZeroMemory(pThis->m_lCookList, sizeof(INT32) * AGCMUICOOK_MAX_LIST);
		pThis->m_lCookCount = pThis->m_pcsAgpmProduct->GetProductList(pThis->m_lCookList, AGCMUICOOK_MAX_LIST,
																   AGPDPRODUCT_CATEGORY_COOK);
		pThis->m_bIsWindowOpen	= TRUE;
	}

	return TRUE;
}


//	
//==========================================================
//
BOOL AgcmUICook::CBMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICook			*pThis				= (AgcmUICook *)	pClass;
	AgcdUIControl		*pcsControl			= pcsSourceControl;

	if (!pcsControl ||
		!pcsControl->m_pcsBase ||
		AcUIBase::TYPE_GRID != pcsControl->m_lType)
		return FALSE;

	AcUIGrid				*pcsUIGrid			= (AcUIGrid *) pcsControl->m_pcsBase;
	AgpdGridSelectInfo		*pstGridSelectInfo	= pcsUIGrid->GetDragDropMessageInfo();
	if (!pstGridSelectInfo || !pstGridSelectInfo->pGridItem || AGPDGRID_ITEM_TYPE_ITEM != pstGridSelectInfo->pGridItem->m_eType)
		return FALSE;

	AgpdGrid *pcsControlGrid = pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsControl);
	if (!pcsControlGrid)
		return FALSE;

	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem(pstGridSelectInfo->pGridItem->m_lItemID);
	if (!pcsItem)
		return FALSE;

	if (AGPDITEM_STATUS_INVENTORY == pcsItem->m_eStatus)
		{
		// ASSERT category == COOK

		// 해당 아이템 제조에 필요한 레시피가 아니면 무시한다.
		if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lID != 
			pThis->m_pcsAgpmProduct->GetProductData(pThis->m_lSelectedCookID)->m_csReceipe.m_pcsItemTemplate->m_lID )
			return FALSE;

		pThis->m_lReceipeID = pcsItem->m_lID;

			// 인벤토리에서 Receipe Grid로 옮겨왔다.
		if (AGPMITEM_TYPE_USABLE != ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType ||
			AGPMITEM_USABLE_TYPE_RECEIPE != ((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType)
			return FALSE;

		pThis->m_pcsAgpmGrid->AddItem(&pThis->m_GridSourceItem, pcsItem->m_pcsGridItem);
		pThis->m_pcsAgpmGrid->Add(&pThis->m_GridSourceItem, pcsItem->m_pcsGridItem, 1, 1);
		pThis->RefreshSourceItemGrid();
		}
	else if (pcsItem->m_eStatus == AGPDITEM_STATUS_RECEIPE)
		{

		}

	return TRUE;
	}


//	Result callback
//=======================================================
//
BOOL AgcmUICook::CBProductResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgcmUICook *pThis = (AgcmUICook *) pClass;

	INT32 lProductID	= (INT32) ((PVOID *)pData)[0];
	INT32 lExp			= (INT32) ((PVOID *)pData)[1];

	eAgpmProductResultCode eResult	 = *(eAgpmProductResultCode *) pCustData;
	CHAR szBuffer[512];
	memset(szBuffer, 0, 512);

	BOOL fUI = FALSE;
	CHAR *psz = NULL;


	switch (eResult)
		{
		case AGPMPRODUCT_RESULT_SUCCESS:				// COMPOSE 성공으로 인한 아이템 획득.
			{
				// ##### 사운드 땜시 이벤트로 처리해야 한다.
				fUI = TRUE;
				AgpdItemTemplate *pcsTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(lProductID);
				if (NULL != pcsTemplate)
				{
					psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_SUCCESS);
					if (psz)
					{
						sprintf(szBuffer, psz, "요리 만들기");
						pThis->AddSystemMessage(szBuffer);
					}

					psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_ITEM);
					if (psz)
					{
						sprintf(szBuffer, psz, pcsTemplate->m_szName, 1);
						pThis->AddSystemMessage(szBuffer);
					}

					psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_EXP);
					if (psz)
					{
						sprintf(szBuffer, psz, "요리 만들기", lExp);
						pThis->AddSystemMessage(szBuffer);
					}

					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_GATHER_SUCCESS]);
				}
			}
			break;

		case AGPMPRODUCT_RESULT_FAIL:
			{
				fUI = TRUE;
				psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_FAIL);
				if (psz)
				{
					sprintf(szBuffer, psz, "요리 만들기");
					pThis->AddSystemMessage(szBuffer);
				}
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_MAKE_FAIL]);
			}
			break;

		case AGPMPRODUCT_RESULT_SUCCESS_MEAT:
			{
				AgpdItemTemplate *pcsTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(lProductID);
				if (NULL != pcsTemplate)
				{
					psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_SUCCESS);
					if (psz)
					{
						sprintf(szBuffer, psz, "고기 잘라내기");
						pThis->AddSystemMessage(szBuffer);
					}

					psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_ITEM);
					if (psz)
					{
						sprintf(szBuffer, psz, pcsTemplate->m_szName, 1);
						pThis->AddSystemMessage(szBuffer);
					}

					psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_EXP);
					if (psz)
					{
						sprintf(szBuffer, psz, "고기 잘라내기", lExp);
						pThis->AddSystemMessage(szBuffer);
					}

					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_GATHER_SUCCESS]);
				}
			}
			break;

		case AGPMPRODUCT_RESULT_SUCCESS_LEATHER:
			{
				AgpdItemTemplate *pcsTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(lProductID);
				if (NULL != pcsTemplate)
				{
					psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_SUCCESS);
					if (psz)
					{
						sprintf(szBuffer, psz, "가죽 벗기기");
						pThis->AddSystemMessage(szBuffer);
					}

					psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_ITEM);
					if (psz)
					{
						sprintf(szBuffer, psz, pcsTemplate->m_szName, 1);
						pThis->AddSystemMessage(szBuffer);
					}

					psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_EXP);
					if (psz)
					{
						sprintf(szBuffer, psz, "가죽 벗기기", lExp);
						pThis->AddSystemMessage(szBuffer);
					}

					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_GATHER_SUCCESS]);
				}
			}
			break;

		case AGPMPRODUCT_RESULT_FAIL_MEAT:
			{
				psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_FAIL);
				if (psz)
				{
					sprintf(szBuffer, psz, "고기 잘라내기");
					pThis->AddSystemMessage(szBuffer);
				}
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_GATHER_FAIL]);
			}
			break;

		case AGPMPRODUCT_RESULT_FAIL_LEATHER:
			{
				psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUICOOK_MESSAGE_FAIL);
				if (psz)
				{
					sprintf(szBuffer, psz, "가죽 벗기기");
					pThis->AddSystemMessage(szBuffer);
				}
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_GATHER_FAIL]);
			}
			break;

		default:
			break;
		}

	if (fUI)
	{
		pThis->ClearGrid();
		pThis->m_bHoldOn = FALSE;	// 다시 버튼 사용 가능함
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUICOOK_EVENT_MAKE_CLOSE]);
		pThis->m_bIsWindowOpen = FALSE;
	}

	return TRUE;
	}

BOOL AgcmUICook::AddSystemMessage(CHAR* pszMsg)
{
	if(!pszMsg)
		return FALSE;

	AgpdChatData	stChatData;
	memset(&stChatData, 0, sizeof(stChatData));

	stChatData.eChatType = AGPDCHATTING_TYPE_SYSTEM_LEVEL1;
	stChatData.szMessage = pszMsg;

	stChatData.lMessageLength = strlen(stChatData.szMessage);
		
	m_pcsAgcmChatting->AddChatMessage(AGCMCHATTING_TYPE_SYSTEM, &stChatData);
	m_pcsAgcmChatting->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);

	return TRUE;
}

BOOL AgcmUICook::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUICook *pThis = (AgcmUICook *) pClass;
	
	pThis->m_bIsWindowOpen = FALSE;
	pThis->m_bHoldOn = FALSE;

	return TRUE;
	}

BOOL AgcmUICook::CBCloseAllUIToolTip(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUICook *pThis = (AgcmUICook *) pClass;

	pThis->m_csToolTip.DeleteAllStringInfo();

	return TRUE;
	}
