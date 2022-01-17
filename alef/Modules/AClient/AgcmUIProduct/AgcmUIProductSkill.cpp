/*=================================================================

	AgcmUIProductSkill.cpp
	
=================================================================*/

#include "AgcmUIProductSkill.h"
#include "AuStrTable.h"

/********************************************************/
/*		The Definition of AgcmUIProductSkill class		*/
/********************************************************/
//
CHAR AgcmUIProductSkill::s_szEvents[AGCMUIPRODUCTSKILL_EVENT_MAX][64] = 
	{
	"PDSK_EVENT_OPEN",
	"PDSK_EVENT_CLOSE",
	"PDSK_EVENT_OPEN_NPCDIALOG",
	"PDSK_EVENT_CLOSE_NPCDIALOG",
	"PDSK_EVENT_BUY_ACTIVE_ITEM",
	"PDSK_EVENT_BUY_DISABLE_ITEM",
	"PDSK_EVENT_BUY_LEARNED_ITEM",
	"PDSK_EVENT_BUY_SELECTED_ITEM",
	"PDSK_EVENT_BUY_UNSELECTED_ITEM",
	"PDSK_EVENT_BUY_SUCCESS",
	"PDSK_EVENT_BUY_FAIL",
	"PDSK_EVENT_SUCCESS",
	"PDSK_EVENT_FAIL",
	};


AgcmUIProductSkill::AgcmUIProductSkill()
	{
	SetModuleName("AgcmUIProductSkill");

	m_pAgpmGrid					= NULL;
	m_pAgpmFactors				= NULL;
	m_pAgpmCharacter			= NULL;
	m_pAgpmItem					= NULL;
	m_pAgpmSkill				= NULL;
	m_pAgpmProduct				= NULL;
	m_pAgcmCharacter			= NULL;
	m_pAgcmItem					= NULL;
	m_pAgcmSkill				= NULL;
	m_pAgcmProduct				= NULL;
	m_pAgcmEventProduct			= NULL;
	m_pAgcmUIManager2			= NULL;
	m_pAgcmUICharacter			= NULL;
	m_pAgcmUIMain				= NULL;
	m_pAgcmChatting				= NULL;
	m_pAgcmUIItem				= NULL;
	m_pAgcmUISkill2				= NULL;

	m_pUDGrid					= NULL;
	m_pUDBuyGrid				= NULL;
	m_pUDInvenMoney				= NULL;
	m_pUDActiveBuyButton		= NULL;
	m_pUDTitle					= NULL;

	ZeroMemory(m_lList, sizeof(INT32) *AGCMUIPRODUCTSKILL_MAX_LIST);
	m_nListCount				= 0;
	m_pSelfCharacter			= NULL;
	m_pApdEvent					= NULL;
	m_llInvenMoney				= 0;
	m_lBuySelectedItemIndex		= (-1);
	m_lDummy					= 0;
	ZeroMemory(&m_stPosition, sizeof(AuPOS));
	m_bIsOpen					= FALSE;
	m_bIsBuyOpen				= FALSE;
	}




AgcmUIProductSkill::~AgcmUIProductSkill()
	{
	}




//	ApModule inherited
//===============================================
//
BOOL AgcmUIProductSkill::OnAddModule()
	{
	m_pAgpmGrid				= (AgpmGrid *)				GetModule("AgpmGrid");
	m_pAgpmFactors			= (AgpmFactors *)			GetModule("AgpmFactors");
	m_pAgpmCharacter		= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pAgpmItem				= (AgpmItem *)				GetModule("AgpmItem");
	m_pAgpmSkill			= (AgpmSkill *)				GetModule("AgpmSkill");
	m_pAgpmProduct			= (AgpmProduct *)			GetModule("AgpmProduct");
	m_pAgpmEventProduct		= (AgpmEventProduct *)		GetModule("AgpmEventProduct");
	m_pAgcmCharacter		= (AgcmCharacter *)			GetModule("AgcmCharacter");
	m_pAgcmItem				= (AgcmItem *)				GetModule("AgcmItem");
	m_pAgcmSkill			= (AgcmSkill *)				GetModule("AgcmSkill");
	m_pAgcmProduct			= (AgcmProduct *)			GetModule("AgcmProduct");
	m_pAgcmEventProduct		= (AgcmEventProduct *)		GetModule("AgcmEventProduct");
	m_pAgcmUIManager2		= (AgcmUIManager2 *)		GetModule("AgcmUIManager2");
	m_pAgcmUICharacter		= (AgcmUICharacter *)		GetModule("AgcmUICharacter");
	m_pAgcmUIMain			= (AgcmUIMain *)			GetModule("AgcmUIMain");
	m_pAgcmChatting			= (AgcmChatting2 *)			GetModule("AgcmChatting2");
	m_pAgcmUIItem			= (AgcmUIItem *)			GetModule("AgcmUIItem");
	m_pAgcmUISkill2			= (AgcmUISkill2 *)			GetModule("AgcmUISkill2");

	if (!m_pAgpmGrid ||
		!m_pAgpmFactors ||
		!m_pAgpmCharacter ||
		!m_pAgpmItem ||
		!m_pAgpmSkill ||
		!m_pAgpmProduct ||
		!m_pAgpmEventProduct ||
		!m_pAgcmCharacter ||
		!m_pAgcmItem ||
		!m_pAgcmSkill ||
		!m_pAgcmProduct ||
		!m_pAgcmEventProduct ||
		!m_pAgcmUIManager2 ||
		!m_pAgcmUICharacter ||
		!m_pAgcmUIMain
		)
		return FALSE;

	if (!m_pAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;
	if (!m_pAgcmCharacter->SetCallbackReleaseSelfCharacter(CBReleaseSelfCharacter, this))
		return FALSE;
	if (!m_pAgpmCharacter->SetCallbackUpdateMoney(CBUpdateInvenMoney, this))
		return FALSE;

	if (!m_pAgpmSkill->SetCallbackInitSkill(CBAddSkill, this))
		return FALSE;
	if (!m_pAgpmSkill->SetCallbackUpdateSkillPoint(CBUpdateSkillPoint, this))
		return FALSE;
	if (!m_pAgpmSkill->SetCallbackUpdateSkill(CBUpdateSkillPoint, this))
		return FALSE;
	if (!m_pAgpmSkill->SetCallbackReceiveAction(CBReceiveSkillAction, this))
		return FALSE;		
	if (!m_pAgpmEventProduct->SetCallbackBuyComposeResult(CBBuyComposeResult, this))
		return FALSE;
	if (!m_pAgpmEventProduct->SetCallbackLearnSkillResult(CBLearnSkillResult, this))
		return FALSE;		
	if (!m_pAgcmEventProduct->SetCallbackGrant(CBGrant, this))
		return FALSE;
	if (!m_pAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	if (m_pAgcmUIMain && !m_pAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	if (!AddFunction() ||
		!AddEvent() ||
		!AddUserData() ||
		!AddDisplay() ||
		!AddBoolean()
		)
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIProductSkill::OnInit()
	{
	// initialize tooltip
	m_csTooltip.m_Property.bTopmost = TRUE;
	m_pAgcmUIManager2->AddWindow((AgcWindow *) (&m_csTooltip));
	m_csTooltip.ShowWindow(FALSE);
	
	// product skill grid
	int i = 0;
	for (i = 0; i < AGCMUIPRODUCTSKILL_MAX_LIST; ++i)
		{
		m_pAgpmGrid->Init(&m_AgpdGridList[i], 1, 1, 1);
		m_AgpdGridList[i].m_lGridType = AGPDGRID_ITEM_TYPE_SKILL;
		}

	// product skill buy grid
	for (i = 0; i < AGCMUIPRODUCTSKILL_MAX_LIST; ++i)
		{
		m_pAgpmGrid->Init(&m_AgpdGridBuyList[i], 1, 1, 1);
		m_AgpdGridBuyList[i].m_lGridType = AGPDGRID_ITEM_TYPE_SKILL;
		}

	return TRUE;
	}


BOOL AgcmUIProductSkill::OnDestroy()
	{
	for (int i = 0; i < AGCMUIPRODUCTSKILL_MAX_LIST; ++i)
		{
		m_pAgpmGrid->Remove(&m_AgpdGridList[i]);
		m_pAgpmGrid->Remove(&m_AgpdGridBuyList[i]);
		}

	return TRUE;
	}


BOOL AgcmUIProductSkill::AddFunction()
	{
	if (!m_pAgcmUIManager2->AddFunction(this, "PDSkill_Open", CBOpen, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "PDSkill_Close", CBClose, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "PDSkill_OpenTooltip", CBOpenTooltip, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "PDSkill_CloseTooltip", CBCloseTooltip, 0))
		return FALSE;		
	if (!m_pAgcmUIManager2->AddFunction(this, "PDSkill_BuyOpen", CBBuyOpen, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "PDSkill_BuySelectGrid", CBBuySelectGrid, 1, "grid user data (parent user data)"))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "PDSkill_BuyBuy", CBBuyBuy, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "PDSkill_BuyUpdateListItem", CBBuyUpdateListItem, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "PDSkill_BuyClose", CBBuyClose, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "PDSkill_BuyOpenTooltip", CBBuyOpenTooltip, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "PDSkill_BuyCloseTooltip", CBBuyCloseTooltip, 0))
		return FALSE;		

	return TRUE;
	}




//	OnAddModule helper
//=================================================
//
BOOL AgcmUIProductSkill::AddEvent()
	{
	for (INT32 i=0; i<AGCMUIPRODUCTSKILL_EVENT_MAX; ++i)
		{
		m_lEvents[i] = m_pAgcmUIManager2->AddEvent(s_szEvents[i]);
		if (m_lEvents[i] < 0)
			return FALSE;
		}

	m_lEventConfirm = m_pAgcmUIManager2->AddEvent("PDSK_EVENT_BUY_CONFIRM", CBConfirmBuy, this);
	if (m_lEventConfirm < 0)
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIProductSkill::AddUserData()
	{
	m_pUDGrid = m_pAgcmUIManager2->AddUserData("PDSK_LISTGRID", m_AgpdGridList, sizeof(AgpdGrid),
											   0, AGCDUI_USERDATA_TYPE_GRID);
	if (!m_pUDGrid)
		return FALSE;
		
	m_pUDBuyGrid = m_pAgcmUIManager2->AddUserData("PDSK_BUYLISTGRID", m_AgpdGridBuyList, sizeof(AgpdGrid),
												  0, AGCDUI_USERDATA_TYPE_GRID);
	if (!m_pUDBuyGrid)
		return FALSE;
													   
	m_pUDInvenMoney = m_pAgcmUIManager2->AddUserData("PDSK_BUYINVENMONEY", &m_llInvenMoney, sizeof(INT64),
													 1, AGCDUI_USERDATA_TYPE_UINT64);
	if (!m_pUDInvenMoney)
		return FALSE;
											
	m_pUDActiveBuyButton = m_pAgcmUIManager2->AddUserData("PDSK_BUYACTIVEBUYBTN", &m_lDummy, sizeof(INT32),
														  1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pUDActiveBuyButton)
		return FALSE;

	m_pUDTitle = m_pAgcmUIManager2->AddUserData("PDSK_TITLE", NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pUDTitle)
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIProductSkill::AddDisplay()
	{
	if (!m_pAgcmUIManager2->AddDisplay(this, "PDSKName", AGCMUIPRODUCTSKILL_DISPLAY_ID_NAME, CBDisplayName, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, "PDSKLvExp", AGCMUIPRODUCTSKILL_DISPLAY_ID_LVEXP, CBDisplayLvExp, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, "PDSKLvExpDegree", AGCMUIPRODUCTSKILL_DISPLAY_ID_EXP_DEGREE, CBDisplayExpBar, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;	
	if (!m_pAgcmUIManager2->AddDisplay(this, "PDSKLvExpDegreeMax", AGCMUIPRODUCTSKILL_DISPLAY_ID_EXP_DEGREE_MAX, CBDisplayExpBar, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;		
	if (!m_pAgcmUIManager2->AddDisplay(this, "PDSKBuyName", AGCMUIPRODUCTSKILL_DISPLAY_ID_BUY_NAME, CBDisplayBuyName, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, "PDSKBuyPrice", AGCMUIPRODUCTSKILL_DISPLAY_ID_BUY_PRICE, CBDisplayBuyPrice, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, "PDSKBuyInvenMoney", AGCMUIPRODUCTSKILL_DISPLAY_ID_BUY_INVENMONEY, CBDisplayBuyInvenMoney, AGCDUI_USERDATA_TYPE_UINT64))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, "PDSKBuyTitle", AGCMUIPRODUCTSKILL_DISPLAY_ID_BUY_TITLE, CBDisplayTitle, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	return TRUE;
	}


BOOL AgcmUIProductSkill::AddBoolean()
	{
	if (!m_pAgcmUIManager2->AddBoolean(this, "PDSKActiveBuyBtn", CBIsActiveBuyButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
	}




//	Init UI
//========================================================
//
BOOL AgcmUIProductSkill::SetList()
	{
	INT32 lAdded = 0;

	for (INT32 i=0; i < AGCMUIPRODUCTSKILL_MAX_LIST; ++i)
		{
		m_pAgpmGrid->Reset(&m_AgpdGridList[i]);
		AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkill(m_lList[i]);
		if (pAgpdSkill && pAgpdSkill->m_pcsTemplate)
			{
			//m_pAgcmSkill->SetGridSkillAttachedTexture((AgpdSkillTemplate *)pAgpdSkill->m_pcsTemplate);
			
			if (m_pAgpmGrid->Add(&m_AgpdGridList[i], 0, 0, 0, pAgpdSkill->m_pcsGridItem, 1, 1))
				++lAdded;

			/*
			AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkillByTID(m_pSelfCharacter, pAgpdSkillTemplate->m_lID);
			if (pAgpdSkill)
				pAgpdSkillTemplate->m_pcsGridItem->m_lItemID = pAgpdSkill->m_lID;
			*/
			}
		}

	m_pUDGrid->m_stUserData.m_lCount = lAdded;

	m_pAgcmUIManager2->SetUserDataRefresh(m_pUDGrid);
	m_pAgcmUIManager2->RefreshUserData(m_pUDGrid);

	return TRUE;	
	}


BOOL AgcmUIProductSkill::SetBuyList()
	{
	m_lBuySelectedItemIndex	= (-1);

	INT32 lAdded = 0;
	INT32 lSkillList[10] = {0, };
	INT32 lSkill = 0;
	INT32 lComposeList[100] = {0, };
	INT32 lCompose = 0;
	INT32 i;

	if (AGPMPRODUCT_CATEGORY_NONE == m_eCategory)
		{
		return FALSE;
		}

	// 해당 카테고리의 스킬과 Compose List를 가져온다.
	lSkill = m_pAgpmProduct->GetSkillList(m_eCategory, lSkillList, 10);
	lCompose = m_pAgpmProduct->GetComposeList(m_eCategory, lComposeList, 100);
	
	// skill
	for (i=0; (i<lSkill && i<AGCMUIPRODUCTSKILL_MAX_LIST); ++i)
		{
		m_AgpdGridBuyList[lAdded].m_lGridType = AGPDGRID_ITEM_TYPE_SKILL;
		m_pAgpmGrid->Reset(&m_AgpdGridBuyList[lAdded]);

		// skill
		AgpdSkillTemplate *pAgpdSkillTemplate = m_pAgpmSkill->GetSkillTemplate(lSkillList[i]);
		if (pAgpdSkillTemplate && m_pAgcmSkill)
			{
			m_pAgcmSkill->SetGridSkillAttachedTexture(pAgpdSkillTemplate);
			AgcdSkillTemplate *pAgcdSkillTemplate = m_pAgcmSkill->GetADSkillTemplate(pAgpdSkillTemplate);
			if (pAgcdSkillTemplate && pAgcdSkillTemplate->m_pSkillTexture)
				RwTextureAddRef(pAgcdSkillTemplate->m_pSkillTexture);
			
			if (m_pAgpmGrid->Add(&m_AgpdGridBuyList[lAdded], 0, 0, 0, pAgpdSkillTemplate->m_pcsGridItem, 1, 1))
				++lAdded;
			//pAgpdSkillTemplate->m_pcsGridItem->m_lItemTID = pAgpdSkillTemplate->m_lID;
			//pAgpdGridItem->m_lItemTID = pAgpdSkillTemplate->m_lID;
			/*
			AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkillByTID(m_pSelfCharacter, pAgpdSkillTemplate->m_lID);
			if (pAgpdSkill)
				pAgpdSkillTemplate->m_pcsGridItem->m_lItemID = pAgpdSkill->m_lID;
			*/
			}
		}

	// compose
	INT32 lAddedSkill = lAdded;
	for (i=0; ((i<lCompose) && i<(AGCMUIPRODUCTSKILL_MAX_LIST-lAddedSkill)); ++i)
		{
		m_AgpdGridBuyList[lAdded].m_lGridType = AGPDGRID_ITEM_TYPE_ITEM;
		m_pAgpmGrid->Reset(&m_AgpdGridBuyList[lAdded]);

		// item
		AgpdItemTemplate *pAgpdItemTemplate = m_pAgpmItem->GetItemTemplate(lComposeList[i]);
		if (pAgpdItemTemplate && m_pAgcmItem)
			{
			m_pAgcmItem->SetGridItemTemplateAttachedTexture(pAgpdItemTemplate, pAgpdItemTemplate->m_pcsGridItem);
			AgcdItemTemplate *pAgcdItemTemplate = m_pAgcmItem->GetTemplateData(pAgpdItemTemplate);
			if (pAgcdItemTemplate && pAgcdItemTemplate->m_pTexture)
				RwTextureAddRef(pAgcdItemTemplate->m_pTexture);
		
			if (m_pAgpmGrid->Add(&m_AgpdGridBuyList[lAdded], 0, 0, 0, pAgpdItemTemplate->m_pcsGridItem, 1, 1))
				++lAdded;
			//pAgpdItemTemplate->m_pcsGridItem->m_lItemTID = pAgpdItemTemplate->m_lID;
			/*
			AgpdItem *pAgpdItem = NULL;//m_pAgpmItem->GetItemGetItemByTID(m_pSelfCharacter, pAgpdItemTemplate->m_lID);
			if (pAgpdItem)
				pAgpdItemTemplate->m_pcsGridItem->m_lItemID = pAgpdItem->m_lID;
			*/
			}		
		}

	m_pUDBuyGrid->m_stUserData.m_lCount = lAdded;
	m_pAgcmUIManager2->SetUserDataRefresh(m_pUDBuyGrid);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pUDActiveBuyButton);
	m_pAgcmUIManager2->RefreshUserData(m_pUDBuyGrid);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pUDInvenMoney);
	m_pAgcmUIManager2->RefreshUserData(m_pUDInvenMoney);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pUDTitle);
	m_pAgcmUIManager2->RefreshUserData(m_pUDTitle);

	for (i = 0; i < lAdded; ++i)
		{
		m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_UNSELECTED_ITEM], i);
		SetStatusBuyItem(i);
		}

	return TRUE;
	}




//	Display helper
//=================================================
//
eAGPMEVENT_PRODUCT_RESULT AgcmUIProductSkill::IsActiveBuyItem(INT32 lIndex)
	{
	if (lIndex < 0 || m_pUDBuyGrid->m_stUserData.m_lCount <= lIndex)
		return AGPMEVENT_PRODUCT_RESULT_FAIL;

	return IsActiveBuyItem(&m_AgpdGridBuyList[lIndex]);
	}


eAGPMEVENT_PRODUCT_RESULT AgcmUIProductSkill::IsActiveBuyItem(AgpdGrid *pAgpdGrid)
	{
	INT32 lResult = AGPMEVENT_PRODUCT_RESULT_FAIL;
	AgpdGridItem *pAgpdGridItem = m_pAgpmGrid->GetItem(pAgpdGrid, 0, 0, 0);
	if (!pAgpdGridItem)
		return (eAGPMEVENT_PRODUCT_RESULT) lResult;

	if (AGPDGRID_ITEM_TYPE_SKILL == pAgpdGrid->m_lGridType)
		{
		AgpdSkillTemplate *pAgpdSkillTemplate = m_pAgpmSkill->GetSkillTemplate(pAgpdGridItem->m_lItemTID);
		if (pAgpdSkillTemplate)
			{
			AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkill(m_pSelfCharacter, pAgpdSkillTemplate->m_szName);
			if (pAgpdSkill)		// already bought
				lResult = AGPMEVENT_PRODUCT_RESULT_FAIL_ALREADY_BOUGHT;
			else
				m_pAgpmEventProduct->IsValidLearnSkill(m_pSelfCharacter, pAgpdSkillTemplate, &lResult);
			}
		}
	else if (AGPDGRID_ITEM_TYPE_ITEM == pAgpdGrid->m_lGridType)
		{
		AgpdComposeTemplate *pAgpdComposeTemplate = m_pAgpmProduct->GetComposeTemplate(pAgpdGridItem->m_lItemTID);
		lResult = AGPMEVENT_PRODUCT_RESULT_FAIL;
		m_pAgpmEventProduct->IsValidBuyCompose(m_pSelfCharacter, pAgpdComposeTemplate, &lResult);
		}
	
	return (eAGPMEVENT_PRODUCT_RESULT) lResult;
	}


BOOL AgcmUIProductSkill::SetStatusBuyItem(INT32 lIndex)
	{
	if (lIndex < 0 || m_pUDBuyGrid->m_stUserData.m_lCount <= lIndex)
		return FALSE;

	switch (IsActiveBuyItem(lIndex))
		{
		case AGPMEVENT_PRODUCT_RESULT_FAIL_ALREADY_BOUGHT:
			m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_LEARNED_ITEM], lIndex);
			break;
		
		case AGPMEVENT_PRODUCT_RESULT_SUCCESS:
			m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_ACTIVE_ITEM], lIndex);
			break;
	
		case AGPMEVENT_PRODUCT_RESULT_FAIL_NOT_ENOUGH_MONEY:
		case AGPMEVENT_PRODUCT_RESULT_FAIL_INSUFFICIENT_LEVEL:
		case AGPMEVENT_PRODUCT_RESULT_FAIL_NEED_PREREQUISITES:
			m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_DISABLE_ITEM], lIndex);
			break;
			
		case AGPMEVENT_PRODUCT_RESULT_FAIL:
		default :
			break;
		}
		
	return TRUE;
	}


BOOL AgcmUIProductSkill::SetRecastSkillTimeToGridItem(INT32 lSkillID, UINT32 ulDuration, UINT32 ulRecastDelay)
	{
	AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkill(lSkillID);
	if (!pAgpdSkill || !pAgpdSkill->m_pcsGridItem)
		return FALSE;

	pAgpdSkill->m_pcsGridItem->SetUseItemTime(GetPrevClockCount(), /*ulDuration + */ulRecastDelay);

	return TRUE;
	}


//	Module callbacks
//==========================================
//
BOOL AgcmUIProductSkill::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis				= (AgcmUIProductSkill *)	pClass;
	AgpdCharacter		*pAgpdCharacter		= (AgpdCharacter *)	pData;

	pThis->m_pSelfCharacter	= pAgpdCharacter;

	ZeroMemory(pThis->m_lList, sizeof(INT32) *AGCMUIPRODUCTSKILL_MAX_LIST);
	pThis->m_nListCount = 0;
	
	AgpdSkillAttachData	*pAgpdSkillAttachData = pThis->m_pAgpmSkill->GetAttachSkillData((ApBase *) pAgpdCharacter);
	if (!pAgpdSkillAttachData)
		return FALSE;
	
	for (INT32 i=0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
		{
		AgpdSkill *pAgpdSkill = pThis->m_pAgpmSkill->GetSkill(pAgpdSkillAttachData->m_alSkillID[i]);
		if (!pAgpdSkill)
			break;

		// if product skill
		if (((AgpdSkillTemplate *) pAgpdSkill->m_pcsTemplate)->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
			pThis->m_lList[pThis->m_nListCount++] = pAgpdSkill->m_lID;
	
		if (pThis->m_nListCount >= AGCMUIPRODUCTSKILL_MAX_LIST)
			break;
		}

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis = (AgcmUIProductSkill *)	pClass;

	pThis->m_pSelfCharacter = NULL;

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBUpdateInvenMoney(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis			= (AgcmUIProductSkill *)	pClass;
	AgpdCharacter		*pAgpdCharacter	= (AgpdCharacter *)	pData;

	pThis->m_pAgpmCharacter->GetMoney(pAgpdCharacter, &pThis->m_llInvenMoney);

	return pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pUDInvenMoney);
	}


BOOL AgcmUIProductSkill::CBAddSkill(PVOID pData, PVOID pClass, PVOID pCustData)	
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis			= (AgcmUIProductSkill *)	pClass;
	AgpdSkill			*pAgpdSkill		= (AgpdSkill *)		pData;

	if (!pThis->m_pSelfCharacter)
		return TRUE;

	if (((AgpdSkillTemplate *) pAgpdSkill->m_pcsTemplate)->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
		{
		BOOL bFind = FALSE;
		for (INT16 i=0; i<pThis->m_nListCount; ++i)
			{
			if (pThis->m_lList[i] == pAgpdSkill->m_lID)
				{
				bFind = TRUE;
				}
			}
		
		if (!bFind)
			{
			pThis->m_lList[pThis->m_nListCount++] = pAgpdSkill->m_lID;
			pThis->SetList();
			}
		}

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBUpdateSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis		= (AgcmUIProductSkill *) pClass;
	AgpdSkill			*pAgpdSkill	= (AgpdSkill *) pData;

	if (!pThis->m_pSelfCharacter)
		return FALSE;

	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pUDGrid);
	pThis->m_pAgcmUIManager2->RefreshUserData(pThis->m_pUDGrid);

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBReceiveSkillAction(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIProductSkill			*pThis			= (AgcmUIProductSkill *)		pClass;
	pstAgpmSkillActionData		pstActionData	= (pstAgpmSkillActionData)	pData;

	AgpdCharacter *pSelfCharacter	= pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (!pSelfCharacter ||
		pSelfCharacter->m_eType != pstActionData->pcsOwnerBase->m_eType ||
		pSelfCharacter->m_lID != pstActionData->pcsOwnerBase->m_lID)
		return TRUE;

	switch (pstActionData->nActionType)
		{
		case AGPMSKILL_ACTION_CAST_SKILL:
		case AGPMSKILL_ACTION_MISS_CAST_SKILL:
			pThis->SetRecastSkillTimeToGridItem(pstActionData->lSkillID, pstActionData->ulDuration, pstActionData->ulRecastDelay);
			break;
		}

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBGrant(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIProductSkill	*pThis			= (AgcmUIProductSkill *) pClass;
	AgpdCharacter		*pAgpdCharacter	= (AgpdCharacter *)	pData;
	ApdEvent			*pApdEvent		= (ApdEvent *) pCustData;

	if (!pApdEvent || !pApdEvent->m_pvData)
		return FALSE;

	AgpdEventProduct *pAgpdEventProduct = (AgpdEventProduct *) pApdEvent->m_pvData;
	pThis->m_eCategory = pAgpdEventProduct->m_eCategory;
	pThis->m_pApdEvent = pApdEvent;

	pThis->m_bIsBuyOpen = TRUE;
	pThis->m_stPosition = pAgpdCharacter->m_stPos;

	return pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvents[AGCMUIPRODUCTSKILL_EVENT_OPEN_NPCDIALOG]);
	}


BOOL AgcmUIProductSkill::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis			= (AgcmUIProductSkill *)			pClass;
	AgpdCharacter		*pAgpdCharacter	= (AgpdCharacter *)			pData;

	if (!pThis->m_bIsBuyOpen)
		return TRUE;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pAgpdCharacter->m_stPos, pThis->m_stPosition);
	if ((INT32) fDistance < AGCMUIPRODUCTSKILL_CLOSE_UI_DISTANCE)
		return TRUE;

	pThis->m_bIsBuyOpen = FALSE;
	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvents[AGCMUIPRODUCTSKILL_EVENT_CLOSE_NPCDIALOG]);

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBBuyComposeResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmUIProductSkill	*pThis			= (AgcmUIProductSkill *)	pClass;
	AgpdCharacter		*pAgpdCharacter	= (AgpdCharacter *) ((PVOID *) pCustData)[0];
	INT32 lResult = (INT32) ((PVOID *) pCustData)[1];	
	INT32 lComposeID = (INT32) ((PVOID *) pCustData)[2];	
	
	return pThis->OnBuyComposeResult(lResult, lComposeID);
	}


BOOL AgcmUIProductSkill::CBLearnSkillResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmUIProductSkill	*pThis			= (AgcmUIProductSkill *)	pClass;
	AgpdCharacter		*pAgpdCharacter	= (AgpdCharacter *) ((PVOID *) pCustData)[0];
	INT32 lResult = (INT32) ((PVOID *) pCustData)[1];	
	INT32 lSkillTID = (INT32) ((PVOID *) pCustData)[2];	
	
	return pThis->OnLearnSkillResult(lResult, lSkillTID);
	}


//	Function callbacks
//===================================================
//
BOOL AgcmUIProductSkill::CBOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis			= (AgcmUIProductSkill *)	pClass;

	if (pThis->m_bIsOpen)
		{
		pThis->m_bIsOpen = FALSE;
		return pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvents[AGCMUIPRODUCTSKILL_EVENT_CLOSE]);
		}
	else
		{
		pThis->m_bIsOpen = TRUE;
		return pThis->SetList();
		}
	}


BOOL AgcmUIProductSkill::CBClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProductSkill *pThis = (AgcmUIProductSkill *)	pClass;

	pThis->m_bIsOpen = FALSE;
	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvents[AGCMUIPRODUCTSKILL_EVENT_CLOSE]);

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBOpenTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl || !pData1)
		return FALSE;

	AgcmUIProductSkill	*pThis = (AgcmUIProductSkill *)	pClass;
	AgcdUIControl		*pControl = (AgcdUIControl *)	pData1;

	AcUIGrid *pGrid = (AcUIGrid *) (pControl->m_pcsBase);
	pGrid->m_pToolTipAgpdGridItem = pThis->m_pAgpmGrid->GetItem(pGrid->m_pAgpdGrid + pcsSourceControl->m_lUserDataIndex, 0, 0, 0);

	if (!pThis->m_pAgcmUISkill2 || !pGrid->m_pToolTipAgpdGridItem ||
		 pGrid->m_pToolTipAgpdGridItem->m_eType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	pThis->m_pAgcmUISkill2->OpenSkillToolTip(pControl, AGCMUI_SKILL2_TOOLTIP_TYPE_NORMAL);
		
	return TRUE;
	}


BOOL AgcmUIProductSkill::CBCloseTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis = (AgcmUIProductSkill *)	pClass;

	if (pThis->m_pAgcmUISkill2)
		pThis->m_pAgcmUISkill2->CBCloseSkillToolTip(pThis->m_pAgcmUISkill2, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	
	
	return TRUE;
	}


BOOL AgcmUIProductSkill::CBBuyOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis			= (AgcmUIProductSkill *)	pClass;

	return pThis->SetBuyList();
	}


BOOL AgcmUIProductSkill::CBBuyClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUIProductSkill	*pThis = (AgcmUIProductSkill *)	pClass;
	pThis->m_bIsBuyOpen = FALSE;
	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvents[AGCMUIPRODUCTSKILL_EVENT_CLOSE_NPCDIALOG]);
		
	return TRUE;
	}


BOOL AgcmUIProductSkill::CBBuySelectGrid(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIProductSkill	*pThis			= (AgcmUIProductSkill *)	pClass;
	AgcdUIControl		*pcsUIControl	= (AgcdUIControl *)	pData1;

	// unselect early item
	if (pThis->m_lBuySelectedItemIndex >= 0)
		pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_UNSELECTED_ITEM], pThis->m_lBuySelectedItemIndex);

	// select new item
	pThis->m_lBuySelectedItemIndex = pcsSourceControl->m_lUserDataIndex;
	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_SELECTED_ITEM], pThis->m_lBuySelectedItemIndex);

	// refresh
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pUDActiveBuyButton);

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBBuyBuy(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis = (AgcmUIProductSkill *)	pClass;

	if (!pThis->m_pApdEvent)
		return FALSE;

	if (pThis->m_lBuySelectedItemIndex < 0)
		{
		//pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_FAIL_NOT_SELECTED]);
		return TRUE;
		}

	if (pThis->m_lBuySelectedItemIndex < 0 || !pThis->m_pApdEvent)
		return FALSE;

	if (AGPMEVENT_PRODUCT_RESULT_SUCCESS != pThis->IsActiveBuyItem(pThis->m_lBuySelectedItemIndex))
		return FALSE;
	
	return pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEventConfirm);
	}


BOOL AgcmUIProductSkill::CBBuyUpdateListItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProductSkill *pThis = (AgcmUIProductSkill *)	pClass;

	for (INT32 i=0; i < pThis->m_pUDBuyGrid->m_stUserData.m_lCount; ++i)
		{
		if (pThis->m_lBuySelectedItemIndex == i)
			pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_SELECTED_ITEM], i);
		else
			pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_UNSELECTED_ITEM], i);

		pThis->SetStatusBuyItem(i);
		}

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBBuyOpenTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl || !pData1)
		return FALSE;

	AgcmUIProductSkill	*pThis = (AgcmUIProductSkill *)	pClass;
	AgcdUIControl		*pControl = (AgcdUIControl *)	pData1;

	if (!pControl->m_pcsBase)
		return FALSE;
	
	AgpdGrid *pAgpdGrid = &(pThis->m_AgpdGridBuyList[pcsSourceControl->m_lUserDataIndex]);
	AgpdGridItem *pAgpdGridItem = pThis->m_pAgpmGrid->GetItem(pAgpdGrid, 0, 0, 0);
	if (!pAgpdGridItem)
		return FALSE;

	if (AGPDGRID_ITEM_TYPE_SKILL == pAgpdGrid->m_lGridType)
		{
		AcUIGrid *pGrid = (AcUIGrid *) (pControl->m_pcsBase);
		pGrid->m_pToolTipAgpdGridItem = pThis->m_pAgpmGrid->GetItem(pGrid->m_pAgpdGrid + pcsSourceControl->m_lUserDataIndex, 0, 0, 0);

		if (!pThis->m_pAgcmUISkill2 || !pGrid->m_pToolTipAgpdGridItem ||
			 pGrid->m_pToolTipAgpdGridItem->m_eType != AGPDGRID_ITEM_TYPE_SKILL)
			return FALSE;

		pThis->m_pAgcmUISkill2->OpenSkillToolTip(pControl, AGCMUI_SKILL2_TOOLTIP_TYPE_NORMAL);
		}
	else if (AGPDGRID_ITEM_TYPE_ITEM == pAgpdGrid->m_lGridType)
		{
		AgpdComposeTemplate *pAgpdComposeTemplate = pThis->m_pAgpmProduct->GetComposeTemplate(pAgpdGridItem->m_lItemTID);
		if (!pAgpdComposeTemplate)
			return FALSE;
					
		pThis->m_csTooltip.MoveWindow((INT32) pThis->m_pAgcmUIManager2->m_v2dCurMousePos.x + 30,
									  (INT32) pThis->m_pAgcmUIManager2->m_v2dCurMousePos.y + 30,
									  pThis->m_csTooltip.w,
									  pThis->m_csTooltip.h);

		pThis->SetTooltipText(&(pThis->m_csTooltip), pAgpdComposeTemplate);
		pThis->m_csTooltip.ShowWindow(TRUE);
		}
		
	return TRUE;
	}


BOOL AgcmUIProductSkill::CBBuyCloseTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIProductSkill	*pThis = (AgcmUIProductSkill *)	pClass;
	
	AgpdGrid *pAgpdGrid = &(pThis->m_AgpdGridBuyList[pcsSourceControl->m_lUserDataIndex]);
	AgpdGridItem *pAgpdGridItem = pThis->m_pAgpmGrid->GetItem(pAgpdGrid, 0, 0, 0);
	if (!pAgpdGridItem)
		return FALSE;

	if (AGPDGRID_ITEM_TYPE_SKILL == pAgpdGrid->m_lGridType)
		{
		if (pThis->m_pAgcmUISkill2)
			pThis->m_pAgcmUISkill2->CBCloseSkillToolTip(pThis->m_pAgcmUISkill2, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		}
	else if (AGPDGRID_ITEM_TYPE_ITEM == pAgpdGrid->m_lGridType)
		{
		pThis->m_csTooltip.ShowWindow(FALSE);
		pThis->m_csTooltip.DeleteAllStringInfo();
		}
	
	return TRUE;
	}




//	Display callbacks
//===============================================
//
BOOL AgcmUIProductSkill::CBDisplayName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
	{
	if (!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUIProductSkill	*pThis		= (AgcmUIProductSkill *)	pClass;
	AgpdGrid			*pAgpdGrid	= (AgpdGrid *)		pData;

	if (pAgpdGrid->m_lGridType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	AgpdGridItem *pAgpdGridItem	= pThis->m_pAgpmGrid->GetItem(pAgpdGrid, 0, 0, 0);
	if (!pAgpdGridItem)
		return FALSE;

	AgpdSkill *pAgpdSkill = pThis->m_pAgpmSkill->GetSkill(pAgpdGridItem->m_lItemID);
	if (pAgpdSkill)
		{
		sprintf(szDisplay, "%s", ((AgpdSkillTemplate *) pAgpdSkill->m_pcsTemplate)->m_szName);
		}
	else
		{
		AgpdSkillTemplate *pAgpdSkillTemplate = pThis->m_pAgpmSkill->GetSkillTemplate(pAgpdGridItem->m_lItemTID);
		if (pAgpdSkillTemplate)
			{
			sprintf(szDisplay, "%s", pAgpdSkillTemplate->m_szName);
			}
		}

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBDisplayLvExp(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
	{
	if (!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUIProductSkill	*pThis			= (AgcmUIProductSkill *)	pClass;
	AgpdGrid			*pAgpdGrid		= (AgpdGrid *)		pData;

	if (pAgpdGrid->m_lGridType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	AgpdGridItem *pAgpdGridItem	= pThis->m_pAgpmGrid->GetItem(pAgpdGrid, 0, 0, 0);
	if (!pAgpdGridItem)
		return FALSE;

	AgpdSkill *pAgpdSkill = pThis->m_pAgpmSkill->GetSkill(pAgpdGridItem->m_lItemID);
	if (pAgpdSkill)
		{
		INT32 lSkillLevel = pThis->m_pAgpmSkill->GetSkillLevel(pAgpdSkill);
		FLOAT fTemp1 = (FLOAT) pThis->m_pAgpmProduct->GetExpOfCurrLevel(pAgpdSkill);
		FLOAT fTemp2 = (FLOAT) pThis->m_pAgpmProduct->GetMaxExpOfCurrLevel(pAgpdSkill);
		FLOAT fExpDegree = (fTemp1 * 100) / fTemp2;
		if (fExpDegree > 100)
			fExpDegree = 100;

		char formatString[128] = {0, };
		if (AP_SERVICE_AREA_WESTERN == g_eServiceArea)
			strcpy(formatString, "%d%s %s:%3.1f%%");
		else
			strcpy(formatString, "%3d%s %s:%4.1f%%");
		
		sprintf(szDisplay,
				formatString,
				lSkillLevel,						// x
				ClientStr().GetStr(STI_GRADE),		// 단계
				ClientStr().GetStr(STI_MASTERY),	// 숙련도
				fExpDegree);						// xx
		}

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBDisplayExpBar(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
	{
	if (!pClass || !pData || !szDisplay || !plValue)
		return FALSE;

	AgcmUIProductSkill	*pThis			= (AgcmUIProductSkill *)	pClass;
	AgpdGrid			*pAgpdGrid		= (AgpdGrid *)		pData;
	INT32				lTemp1 = 0;
	INT32				lTemp2 = 0;

	if (pAgpdGrid->m_lGridType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	AgpdGridItem *pAgpdGridItem	= pThis->m_pAgpmGrid->GetItem(pAgpdGrid, 0, 0, 0);
	if (!pAgpdGridItem)
		return FALSE;

	AgpdSkill *pAgpdSkill = pThis->m_pAgpmSkill->GetSkill(pAgpdGridItem->m_lItemID);
	if (pAgpdSkill)
		{
		switch (lID)
			{	
			case AGCMUIPRODUCTSKILL_DISPLAY_ID_EXP_DEGREE:
				lTemp1 = pThis->m_pAgpmProduct->GetExpOfCurrLevel(pAgpdSkill);
				if (0 > lTemp1)
					lTemp1 = 0;

				lTemp2 = pThis->m_pAgpmProduct->GetMaxExpOfCurrLevel(pAgpdSkill);
				if (lTemp1 > lTemp2)
					lTemp1 = lTemp2;

				*plValue = lTemp1;
				sprintf(szDisplay, "%d", *plValue);
				break;

			case AGCMUIPRODUCTSKILL_DISPLAY_ID_EXP_DEGREE_MAX:
				lTemp1 = pThis->m_pAgpmProduct->GetMaxExpOfCurrLevel(pAgpdSkill);
				*plValue = lTemp1;
				sprintf(szDisplay, "%d", *plValue);
				break;

			default:
				break;
			}
		}
		
	return TRUE;
	}


BOOL AgcmUIProductSkill::CBDisplayBuyName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
	{
	if (!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUIProductSkill	*pThis		= (AgcmUIProductSkill *)	pClass;
	AgpdGrid			*pAgpdGrid	= (AgpdGrid *)		pData;

	AgpdGridItem *pAgpdGridItem	= pThis->m_pAgpmGrid->GetItem(pAgpdGrid, 0, 0, 0);
	if (!pAgpdGridItem)
		return FALSE;

	if (AGPDGRID_ITEM_TYPE_SKILL == pAgpdGrid->m_lGridType)
		{
		AgpdSkillTemplate *pAgpdSkillTemplate = pThis->m_pAgpmSkill->GetSkillTemplate(pAgpdGridItem->m_lItemTID);
		if (pAgpdSkillTemplate)
			{
			sprintf(szDisplay, "%s (%s)", pAgpdSkillTemplate->m_szName, ClientStr().GetStr(STI_BASIC));
			}
		}
	else if (AGPDGRID_ITEM_TYPE_ITEM == pAgpdGrid->m_lGridType)
		{
		AgpdComposeTemplate *pAgpdComposeTemplate = pThis->m_pAgpmProduct->GetComposeTemplate(pAgpdGridItem->m_lItemTID);
		if (pAgpdComposeTemplate)
			{
			sprintf(szDisplay, "%s", pAgpdComposeTemplate->m_szCompose);
			}
		}

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBDisplayBuyPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
	{
	if (!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUIProductSkill	*pThis			= (AgcmUIProductSkill *)	pClass;
	AgpdGrid			*pAgpdGrid		= (AgpdGrid *) pData;

	switch (pThis->IsActiveBuyItem(pAgpdGrid))
		{
		case AGPMEVENT_PRODUCT_RESULT_FAIL_ALREADY_BOUGHT:
			strcpy(szDisplay, ClientStr().GetStr(STI_ALREADY_LEARN));
			break;
	
		case AGPMEVENT_PRODUCT_RESULT_FAIL:

			
		/*
		case AGPMEVENT_PRODUCT_RESULT_SUCCESS:
		case AGPMEVENT_PRODUCT_RESULT_FAIL_NOT_ENOUGH_MONEY:
		case AGPMEVENT_PRODUCT_RESULT_FAIL_INSUFFICIENT_LEVEL:
		case AGPMEVENT_PRODUCT_RESULT_FAIL_NEED_PREREQUISITES:
		*/
		default :
			{
			AgpdGridItem *pAgpdGridItem = pThis->m_pAgpmGrid->GetItem(pAgpdGrid, 0, 0, 0);
			if (!pAgpdGridItem)
				return TRUE;

			INT32 lTax = 0;

			if (AGPDGRID_ITEM_TYPE_SKILL == pAgpdGrid->m_lGridType)
				{
				sprintf(szDisplay, "%s%2d %s%4d%s",
								   ClientStr().GetStr(STI_LEVEL),
								   pThis->m_pAgpmEventProduct->GetLearnableLevel(pAgpdGridItem->m_lItemTID),
								   AP_SERVICE_AREA_WESTERN == g_eServiceArea ? "" : ClientStr().GetStr(STI_PRICE),
								   pThis->m_pAgpmEventProduct->GetBuyCost(pAgpdGridItem->m_lItemTID, pThis->m_pAgcmCharacter->GetSelfCharacter(), &lTax),
								   AP_SERVICE_AREA_WESTERN == g_eServiceArea ? "G" : ClientStr().GetStr(STI_GELD)
								   );
				}
			else if (AGPDGRID_ITEM_TYPE_ITEM == pAgpdGrid->m_lGridType)
				{
				AgpdComposeTemplate *pAgpdComposeTemplate = pThis->m_pAgpmProduct->GetComposeTemplate(pAgpdGridItem->m_lItemTID);
				if (pAgpdComposeTemplate)
					sprintf(szDisplay, "%s%2d %s%4d%s",
							ClientStr().GetStr(STI_MASTERY),
							pAgpdComposeTemplate->m_lLevel,
							AP_SERVICE_AREA_WESTERN == g_eServiceArea ? "" : ClientStr().GetStr(STI_PRICE),
							pAgpdComposeTemplate->m_lPrice,
							AP_SERVICE_AREA_WESTERN == g_eServiceArea ? "G" : ClientStr().GetStr(STI_GELD));
				}
			}
		}

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBDisplayBuyInvenMoney(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis = (AgcmUIProductSkill *)	pClass;

	sprintf(szDisplay, "%I64d", pThis->m_pSelfCharacter ? pThis->m_pSelfCharacter->m_llMoney : 0);
	

	return TRUE;
	}


BOOL AgcmUIProductSkill::CBDisplayTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis = (AgcmUIProductSkill *)	pClass;

	switch (pThis->m_eCategory)
		{
		case AGPMPRODUCT_CATEGORY_HUNT :
			strcpy(szDisplay, ClientStr().GetStr(STI_HUNT));
			break;
		case AGPMPRODUCT_CATEGORY_COOK :
			strcpy(szDisplay, ClientStr().GetStr(STI_COOK));
			break;
		case AGPMPRODUCT_CATEGORY_ALCHEMY :
			strcpy(szDisplay, ClientStr().GetStr(STI_ALCHEMY));
			break;
		default:
			break;
		}	

	return TRUE;
	}




//	Event return callbacks
//===================================================
//
BOOL AgcmUIProductSkill::CBConfirmBuy(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
	{
	if (!pClass)
		return FALSE;

	if (lTrueCancel == (INT32) TRUE)
		{
		AgcmUIProductSkill *pThis = (AgcmUIProductSkill *) pClass;

		if (pThis->m_lBuySelectedItemIndex < 0 || !pThis->m_pApdEvent)
			return FALSE;

		if (AGPMEVENT_PRODUCT_RESULT_SUCCESS != pThis->IsActiveBuyItem(pThis->m_lBuySelectedItemIndex))
			return FALSE;

		AgpdGridItem *pAgpdGridItem	= pThis->m_pAgpmGrid->GetItem(&pThis->m_AgpdGridBuyList[pThis->m_lBuySelectedItemIndex], 0, 0, 0);
		if (!pAgpdGridItem)
			return FALSE;
	
		if (AGPDGRID_ITEM_TYPE_SKILL == pThis->m_AgpdGridBuyList[pThis->m_lBuySelectedItemIndex].m_lGridType)
			return pThis->m_pAgcmEventProduct->SendLearnSkill(pThis->m_pApdEvent, pAgpdGridItem->m_lItemTID);
		else if (AGPDGRID_ITEM_TYPE_ITEM == pThis->m_AgpdGridBuyList[pThis->m_lBuySelectedItemIndex].m_lGridType)
			return pThis->m_pAgcmEventProduct->SendBuyCompose(pThis->m_pApdEvent, pAgpdGridItem->m_lItemTID);
		else
			return FALSE;
		}

	return TRUE;
	}




//	Boolean callbacks
//======================================================
//
BOOL AgcmUIProductSkill::CBIsActiveBuyButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProductSkill	*pThis	= (AgcmUIProductSkill *)	pClass;

	if (pThis->m_lBuySelectedItemIndex < 0)
		return FALSE;

	if (AGPMEVENT_PRODUCT_RESULT_SUCCESS != pThis->IsActiveBuyItem(pThis->m_lBuySelectedItemIndex))
		return FALSE;

	return TRUE;
	}




//	System driven UI close callback
//==================================================
//
BOOL AgcmUIProductSkill::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProductSkill *pThis = (AgcmUIProductSkill *) pClass;
	
	pThis->m_bIsOpen = FALSE;
	pThis->m_bIsBuyOpen = FALSE;

	return TRUE;
	}




//	Open
//============================================
//
BOOL AgcmUIProductSkill::Open()
	{
	return m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_OPEN]);
	}




//	Result
//==========================================================
//
BOOL AgcmUIProductSkill::OnResult(AgpdSkill *pAgpdSkill, INT32 lResult, INT32 lItemTID, INT32 lExp, int nCount )
	{
	if (!pAgpdSkill || !pAgpdSkill->m_pcsTemplate)
		return FALSE;

	AgpdSkillTemplate *pAgpdSkillTemplate = (AgpdSkillTemplate *)pAgpdSkill->m_pcsTemplate;
	CHAR *pszSkillName = pAgpdSkillTemplate->m_szName;
	CHAR sz[512] = _T("");
		
	switch (lResult)
		{
		case AGPMPRODUCT_RESULT_SUCCESS:
			{
			// success
			sprintf(sz, "%s %s", pszSkillName, ClientStr().GetStr(STI_SKILL_SUCCESS));
			AddSystemMessage(sz);
			
			// item
			AgpdItemTemplate *pAgpdItemTemplate = m_pAgpmItem->GetItemTemplate(lItemTID);
			if (pAgpdItemTemplate)
				{
					std::string strItemPost = ClientStr().GetStr(STI_ITEM_POST);
					std::string strNumberUnit = ClientStr().GetStr(STI_NUMBER_UNIT);
					std::string strItemGet = ClientStr().GetStr(STI_ITEM_GET);

				if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
					sprintf(sz, "%s %d %s %s %s ", strItemGet.c_str(), nCount, strNumberUnit.c_str(), pAgpdItemTemplate->m_szName, strItemPost.c_str());
				else if (g_eServiceArea == AP_SERVICE_AREA_WESTERN)
					sprintf(sz, "%d %s %s", nCount, pAgpdItemTemplate->m_szName, ClientStr().GetStr(STI_ITEM_GET));
				else
					sprintf(sz, "%s %s %d %s", pAgpdItemTemplate->m_szName, ClientStr().GetStr(STI_ITEM_POST), nCount, ClientStr().GetStr(STI_ITEM_GET));

				AddSystemMessage(sz);
				}
			
			// exp
			if (lExp > 0)
				{
				sprintf(sz, "%s %s %d %s", pszSkillName, ClientStr().GetStr(STI_SKILLMASTERY), lExp, ClientStr().GetStr(STI_MASTERY_UP));
				AddSystemMessage(sz);
				}
				
			// sound
			m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_SUCCESS]);
			}
			break;
			
		case AGPMPRODUCT_RESULT_FAIL:
			{
			// fail
			sprintf(sz, "%s %s", pszSkillName, ClientStr().GetStr(STI_SKILL_FAIL));
			AddSystemMessage(sz);
			
			// sound
			m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_FAIL]);
			}
			break;
			
		case AGPMPRODUCT_RESULT_FAIL_NO_TOOL:
			{
			AgpdItemTemplate *pAgpdItemTemplate = m_pAgpmItem->GetItemTemplate(
													pAgpdSkillTemplate->m_stConditionArg[0].lArg4);
			if (pAgpdItemTemplate)
				{
				if (g_eServiceArea == AP_SERVICE_AREA_WESTERN)
					sprintf(sz, ClientStr().GetStr(STI_NO_ITEM), pAgpdItemTemplate->m_szName, pszSkillName);
				else
					sprintf(sz, ClientStr().GetStr(STI_NO_ITEM), pszSkillName, pAgpdItemTemplate->m_szName);
				AddSystemMessage(sz);
				}
			}
			break;
			
		case AGPMPRODUCT_RESULT_FAIL_INVALID_TARGET:
			{
			sprintf(sz, ClientStr().GetStr(STI_WRONG_TARGET), pszSkillName);
			AddSystemMessage(sz);
			}
			break;
					
		case AGPMPRODUCT_RESULT_FAIL_TARGET_TOO_FAR:
			{
			sprintf(sz, "%s %s", pszSkillName, ClientStr().GetStr(STI_TOO_FAR));
			AddSystemMessage(sz);
			}
			break;
		
		case AGPMPRODUCT_RESULT_FAIL_INVENTORY_FULL:
			{
			AgpdItemTemplate *pAgpdItemTemplate = m_pAgpmItem->GetItemTemplate(lItemTID);
			if (pAgpdItemTemplate)
				{
				sprintf(sz, "%s %s", pAgpdItemTemplate->m_szName, ClientStr().GetStr(STI_NO_SPACE));
				AddSystemMessage(sz);
				}	
			}
			
		/*case AGPMPRODUCT_RESULT_FAIL_INSUFFICIENT_SOURCE_ITEM:
			{
			AgpdItemTemplate *pAgpdItemTemplate = m_pAgpmItem->GetItemTemplate(lItemTID);
			if (pAgpdItemTemplate)
				{
				sprintf(sz, _T("%s 용 재료가 부족합니다."), pAgpdItemTemplate->m_szName);
				AddSystemMessage(sz);
				}	
			}	*/
		
		case AGPMPRODUCT_RESULT_FAIL_NO_REMAINED_ITEM:
			{
			sprintf(sz, ClientStr().GetStr(STI_NO_GET));
			AddSystemMessage(sz);
			}
			break;
			
		default :
			break;
		}
	
	return TRUE;
	}


BOOL AgcmUIProductSkill::OnBuyComposeResult(INT32 lResult, INT32 lID)
	{
	AgpdComposeTemplate *pAgpdComposeTemplate = m_pAgpmProduct->GetComposeTemplate(lID);
	if (!pAgpdComposeTemplate)
		return FALSE;

	CHAR sz[512] = _T("");

	switch (lResult)
		{
		case AGPMEVENT_PRODUCT_RESULT_SUCCESS:
			{
			sprintf(sz, "%s %s", pAgpdComposeTemplate->m_szCompose, ClientStr().GetStr(STI_SKILL_LEARN));
			AddSystemMessage(sz);			
			
			m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_SUCCESS]);
			}
			break;
		case AGPMEVENT_PRODUCT_RESULT_FAIL:
			break;
		case AGPMEVENT_PRODUCT_RESULT_FAIL_ALREADY_BOUGHT:
			break;
		case AGPMEVENT_PRODUCT_RESULT_FAIL_NOT_ENOUGH_MONEY:
			{
			sprintf(sz, ClientStr().GetStr(STI_CANT_LEARN_NO_GELD));
			AddSystemMessage(sz);			

			m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_FAIL]);
			}
			break;
		case AGPMEVENT_PRODUCT_RESULT_FAIL_INSUFFICIENT_LEVEL:
			break;
		case AGPMEVENT_PRODUCT_RESULT_FAIL_NEED_PREREQUISITES:
			{
			sprintf(sz, ClientStr().GetStr(STI_CANT_LEARN_NEED_SKILL), pAgpdComposeTemplate->m_szCompose);
			AddSystemMessage(sz);			

			m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_FAIL]);		
			}
			break;
		}
	
	//m_pAgcmUIManager2->SetUserDataRefresh(m_pUDBuyGrid);
	//m_pAgcmUIManager2->RefreshUserData(m_pUDBuyGrid);
	SetBuyList();
	
	return TRUE;
	}


BOOL AgcmUIProductSkill::OnLearnSkillResult(INT32 lResult, INT32 lTID)
	{
	AgpdSkillTemplate *pAgpdSkillTemplate = m_pAgpmSkill->GetSkillTemplate(lTID);
	if (!pAgpdSkillTemplate)
		return FALSE;
	
	CHAR sz[512] = _T("");
	
	switch (lResult)
		{
		case AGPMEVENT_PRODUCT_RESULT_SUCCESS:
			{
			sprintf(sz, "%s %s", pAgpdSkillTemplate->m_szName, ClientStr().GetStr(STI_SKILL_LEARN));
			AddSystemMessage(sz);			
			
			m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_SUCCESS]);
			}
			break;
		case AGPMEVENT_PRODUCT_RESULT_FAIL:
			break;
		case AGPMEVENT_PRODUCT_RESULT_FAIL_ALREADY_BOUGHT:
			break;
		case AGPMEVENT_PRODUCT_RESULT_FAIL_NOT_ENOUGH_MONEY:
			{
			sprintf(sz, ClientStr().GetStr(STI_CANT_LEARN_NO_GELD));
			AddSystemMessage(sz);			

			m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_FAIL]);
			}
			break;
		case AGPMEVENT_PRODUCT_RESULT_FAIL_INSUFFICIENT_LEVEL:
			break;
		case AGPMEVENT_PRODUCT_RESULT_FAIL_NEED_PREREQUISITES:
			{
			sprintf(sz, ClientStr().GetStr(STI_CANT_LEARN_NEED_SKILL), pAgpdSkillTemplate->m_szName);
			AddSystemMessage(sz);			

			m_pAgcmUIManager2->ThrowEvent(m_lEvents[AGCMUIPRODUCTSKILL_EVENT_BUY_FAIL]);		
			}
			break;
		}

	//m_pAgcmUIManager2->SetUserDataRefresh(m_pUDBuyGrid);
	//m_pAgcmUIManager2->RefreshUserData(m_pUDBuyGrid);
	SetBuyList();
	
	return TRUE;
	}


BOOL AgcmUIProductSkill::AddSystemMessage(CHAR* pszMsg)
	{
	if(!pszMsg || !m_pAgcmChatting)
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




//	Tooltip
//=================================================
//
BOOL AgcmUIProductSkill::SetTooltipText(AcUIToolTip *pcsTooltip, AgpdComposeTemplate *pAgpdComposeTemplate, BOOL bShowReceipe)
	{
	CHAR	szBuffer[128];
	ZeroMemory(szBuffer, sizeof(CHAR) * 128);

	AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
	if (!pAgpdCharacter || !m_pAgcmUIItem)
		return FALSE;

	// result item
	sprintf(szBuffer, _T("%s"), pAgpdComposeTemplate->m_ResultItem.m_pItemTemplate->m_szName);
	pcsTooltip->AddString(szBuffer, 14, COLOR_WHITE);

	// description
	AgcdUIItemTooltipDesc *pTooltip = m_pAgcmUIItem->GetADItemTemplate(pAgpdComposeTemplate->m_ResultItem.m_pItemTemplate);
	if (strlen(pTooltip->m_szDescription) > 0)
		{
		pcsTooltip->AddNewLine(14);
		pcsTooltip->AddNewLine(14);

		CHAR *pszDesc = pTooltip->m_szDescription;
		CHAR *psz = szBuffer;

		while (TRUE)
			{
			if (_T('\0') == *pszDesc)
				{
				*psz = _T('\0');
				if (*psz != szBuffer[0])
					{
					pcsTooltip->AddString(szBuffer, 14, COLOR_WHITE);
					pcsTooltip->AddNewLine(14);
					}
				break;
				}
			// meet new line
			else if (_T('\\') == *pszDesc && _T('n') == *(pszDesc+1))
				{
				*psz = _T('\0');
				pszDesc += 2;
				psz = szBuffer;
				
				pcsTooltip->AddString(szBuffer, 14, COLOR_WHITE);
				pcsTooltip->AddNewLine(14);
				}
			else
				*psz++ = *pszDesc++;
			}
		}	

	pcsTooltip->AddNewLine(14);
	sprintf(szBuffer, ClientStr().GetStr(STI_MATERIAL));
	pcsTooltip->AddString(szBuffer, 14, COLOR_WHITE);
	pcsTooltip->AddNewLine(14);

	// source item info.
	for (int i = 0; i < pAgpdComposeTemplate->m_lRequiredItems; ++i)
		{
		sprintf(szBuffer, "%s %d%s", pAgpdComposeTemplate->m_RequiredItems[i].m_pItemTemplate->m_szName, 
									 pAgpdComposeTemplate->m_RequiredItems[i].m_lCount, ClientStr().GetStr(STI_NUMBER_UNIT));
		if (m_pAgpmProduct->IsValidItemStatus(m_pAgcmCharacter->GetSelfCharacter(),
											  pAgpdComposeTemplate->m_RequiredItems[i].m_pItemTemplate, 
											  pAgpdComposeTemplate->m_RequiredItems[i].m_lCount))
			pcsTooltip->AddString(szBuffer, 14, COLOR_WHITE);
		else
			pcsTooltip->AddString(szBuffer, 14, COLOR_RED);
		pcsTooltip->AddNewLine(14);
		}

	pcsTooltip->AddNewLine(14);

	// receipe
	if (bShowReceipe && pAgpdComposeTemplate->m_Receipe.m_pItemTemplate)
		{
		sprintf(szBuffer, ClientStr().GetStr(STI_OPTION));
		pcsTooltip->AddString(szBuffer, 14, COLOR_WHITE);
		pcsTooltip->AddNewLine(14);
		
		sprintf(szBuffer, "%s", pAgpdComposeTemplate->m_Receipe.m_pItemTemplate->m_szName);
		if (m_pAgpmProduct->IsValidItemStatus(m_pAgcmCharacter->GetSelfCharacter(),
											  pAgpdComposeTemplate->m_Receipe.m_pItemTemplate,
											  pAgpdComposeTemplate->m_Receipe.m_lCount))
			pcsTooltip->AddString(szBuffer, 14, COLOR_WHITE);
		else
			pcsTooltip->AddString(szBuffer, 14, COLOR_RED);
		pcsTooltip->AddNewLine(14);
		pcsTooltip->AddNewLine(14);
		}

	/*
	// price
	if (pAgpdComposeTemplate->m_lPrice > 0)
		{
		sprintf(szBuffer, "비용 %d겔드 소요", pAgpdComposeTemplate->m_lPrice);
		if (pAgpdCharacter->m_llMoney >= pAgpdComposeTemplate->m_lPrice)
			m_csTooltip.AddString(szBuffer, 14, COLOR_WHITE);
		else
			m_csTooltip.AddString(szBuffer, 14, COLOR_RED);

		m_csTooltip.AddNewLine(14);	
		}
	*/

	return TRUE;
	}