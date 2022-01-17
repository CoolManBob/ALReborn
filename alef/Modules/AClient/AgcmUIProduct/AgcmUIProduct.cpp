/*==============================================================

	AgcmUIProduct.cpp

==============================================================*/

#include "AgcmUIProduct.h"
#include "AgpdProduct.h"
#include "AgcmTitle.h"

#include "AuStrTable.h"


/********************************************************/
/*		The Implementation of AgcmUIProduct class		*/
/********************************************************/
//
CHAR AgcmUIProduct::s_szEvent[AGCMUIPRODUCT_EVENT_MAX][30] = 
	{
	"COMPOSE_EVENT_SELECT_OPEN",
	"COMPOSE_EVENT_SELECT_CLOSE",
	"COMPOSE_EVENT_SELECT_LIST",
	"COMPOSE_EVENT_MAKE",
	"COMPOSE_EVENT_SUCCESS",
	"COMPOSE_EVENT_FAIL"
	};


CHAR AgcmUIProduct::s_szMessage[AGCMUIPRODUCT_MESSAGE_MAX][30] = 
	{
	"COMPOSE_TITLE",
	"COMPOSE_SELECT_EDIT",
	"COMPOSE_MAKE_TITLE",
	"COMPOSE_MAKE_NAME",
	"COMPOSE_MAKE_LEVEL",
	"COMPOSE_MAKE_PRICE",
	"COMPOSE_MAKE_OK",
	"COMPOSE_MAKE_CANCEL"
	};

// 표시를 위해서 추가 by ST
CHAR* AgcmUIProduct::s_szL10NCategory[AGPMPRODUCT_CATEGORY_MAX] = 
	{
	NULL,
	};

AgcmUIProduct::AgcmUIProduct()
	{
	SetModuleName("AgcmUIProduct");

	m_pAgpmFactors			= NULL;
	m_pAgpmCharacter		= NULL;
	m_pAgcmCharacter		= NULL;
	m_pAgcmUIManager2		= NULL;
	m_pAgpmProduct			= NULL;
	m_pAgpmGrid				= NULL;
	m_pAgpmItem				= NULL;
	m_pAgpmSkill			= NULL;
	m_pAgcmItem				= NULL;
	m_pAgcmUIControl		= NULL;
	m_pAgcmProduct			= NULL;
	m_pAgcmChatting			= NULL;
	m_pAgcmUICooldown		= NULL;
	m_pAgcmUIProductSkill	= NULL;

	// user data
	m_pstSelectList			= NULL;
	m_pstMakeEdit			= NULL;
	m_pstMakeSourceItem		= NULL;
	m_pstActiveComposeButton = NULL;

	m_lComposeBtn			= 0;
	
	ZeroMemory(m_lComposeList, sizeof(INT32) * AGCMUIPRODUCT_MAX_LIST);
	ZeroMemory(m_pGridItemList, sizeof(AgpdGridItem *) * AGCMUIPRODUCT_MAX_GRID);

	m_eCategory				= AGPMPRODUCT_CATEGORY_NONE;
	m_lSkillID				= 0;
	m_lSelectedComposeID	= 0;
	m_lReceipeID			= 0;

	m_bHold					= FALSE;
	m_bIsWindowOpen			= FALSE;
	m_lCooldownID = AGCMUICOOLDOWN_INVALID_ID;

	s_szL10NCategory[0] = ClientStr().GetStr( STI_HUNT );
	s_szL10NCategory[1] = ClientStr().GetStr( STI_COOK );
	s_szL10NCategory[2] = ClientStr().GetStr( STI_ALCHEMY );
	}


AgcmUIProduct::~AgcmUIProduct()
	{
	}




//	ApModule inherited
//===============================================
//
BOOL AgcmUIProduct::OnInit()
	{
	// initialize tooltip
	m_csTooltip.m_Property.bTopmost = TRUE;
	m_pAgcmUIManager2->AddWindow((AgcWindow *) (&m_csTooltip));
	m_csTooltip.ShowWindow(FALSE);
	
	// initialize grid
	m_pAgpmGrid->Init(&m_GridSourceItem, 1, 2, 4);
	m_GridSourceItem.m_lGridType = AGPDGRID_ITEM_TYPE_ITEM;
	
	return TRUE;
	}


BOOL AgcmUIProduct::OnDestroy()
	{
	ClearGrid();
	m_pAgpmGrid->Remove(&m_GridSourceItem);
	return TRUE;
	}


BOOL AgcmUIProduct::OnAddModule()
	{
	m_pAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgcmCharacter	= (AgcmCharacter *) GetModule("AgcmCharacter");
	m_pAgpmProduct		= (AgpmProduct *) GetModule("AgpmProduct");
	m_pAgcmUIManager2	= (AgcmUIManager2 *) GetModule("AgcmUIManager2");
	m_pAgpmGrid			= (AgpmGrid *) GetModule("AgpmGrid");
	m_pAgpmItem			= (AgpmItem *) GetModule("AgpmItem");
	m_pAgpmSkill		= (AgpmSkill *) GetModule("AgpmSkill");
	m_pAgcmItem			= (AgcmItem *) GetModule("AgcmItem");
	m_pAgcmUIControl	= (AgcmUIControl *) GetModule("AgcmUIControl");
	m_pAgpmFactors		= (AgpmFactors *) GetModule("AgpmFactors");
	m_pAgcmProduct		= (AgcmProduct *) GetModule("AgcmProduct");
	m_pAgcmChatting		= (AgcmChatting2 *) GetModule("AgcmChatting2");
	m_pAgcmUIMain		= (AgcmUIMain *) GetModule("AgcmUIMain");
	m_pAgcmUICooldown	= (AgcmUICooldown *) GetModule("AgcmUICooldown");
	m_pAgcmUIProductSkill = (AgcmUIProductSkill *) GetModule("AgcmUIProductSkill");
	m_pAgcmSkill		= (AgcmSkill *) GetModule("AgcmSkill");

	if (!m_pAgpmCharacter ||!m_pAgcmCharacter || !m_pAgpmProduct ||
		!m_pAgcmUIManager2 || !m_pAgpmGrid || !m_pAgcmItem ||
		!m_pAgcmUIControl || !m_pAgpmItem || !m_pAgpmFactors ||
		!m_pAgcmProduct || !m_pAgcmChatting || !m_pAgcmSkill
		)
		return FALSE;
	
	if (m_pAgcmUIMain && !m_pAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	if (m_pAgcmUIMain && !m_pAgcmUIMain->SetCallbackCloseAllUITooltip(CBCloseAllUIToolTip, this))
		return FALSE;

	if (m_pAgcmUICooldown)
		{
		m_lCooldownID = m_pAgcmUICooldown->RegisterCooldown(CBAfterCooldown, this);
		if (AGCMUICOOLDOWN_INVALID_ID >= m_lCooldownID)
			return FALSE;
		}

	if (!AddEvent() || !AddFunction() || !AddDisplay() || !AddUserData() || !AddBoolean())
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIProduct::AddEvent()
	{
	for (int i=0; i < AGCMUIPRODUCT_EVENT_MAX; ++i)
		{
		m_lEvent[i] = m_pAgcmUIManager2->AddEvent(s_szEvent[i]);
		if (m_lEvent[i] < 0)
			return FALSE;
		}

	return TRUE;	
	}


BOOL AgcmUIProduct::AddFunction()
	{
	if (!m_pAgcmUIManager2->AddFunction(this, "COMPOSE_OPEN", CBOpenWindow, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "COMPOSE_LIST_ITEM_CLICK", CBClickListItem, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "COMPOSE_TOOLTIP_OPEN", CBOpenTooltip, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "COMPOSE_TOOLTIP_CLOSE", CBCloseTooltip, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "COMPOSE_OKBUTTON_CLICK", CBClickOkButton, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "COMPOSE_CLOSE", CBCloseWindow, 0))
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIProduct::AddDisplay()
	{
	if (!m_pAgcmUIManager2->AddDisplay(this, "COMPOSE_DISP_TITLE", 0, CBDisplayTitle, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "COMPOSE_DISP_SEL_EDIT", 0, CBDisplaySelectEdit, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "COMPOSE_DISP_SEL_LIST", 0, CBDisplaySelectList, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "COMPOSE_DISP_MAKE_EDIT", 0, CBDisplayMakeEdit, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "COMPOSE_DISP_MAKE_OK", 0, CBDisplayMakeOk, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "COMPOSE_DISP_MAKE_CANCEL", 0, CBDisplayMakeCancel, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIProduct::AddUserData()
	{
	// for title, select edit, ok button, cancel button refresh
	m_pstDummy = m_pAgcmUIManager2->AddUserData("COMPOSE_UD_DUMMY", NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstDummy)
		return FALSE;

	// select list userdata
	m_pstSelectList = m_pAgcmUIManager2->AddUserData("COMPOSE_UD_SEL_LIST", m_lComposeList, sizeof(INT32), AGCMUIPRODUCT_MAX_PAGE_LIST, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstSelectList)
		return FALSE;

	// make edit(selected item description) userdata
	m_pstMakeEdit = m_pAgcmUIManager2->AddUserData("COMPOSE_UD_MAKE_EDIT", NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstMakeEdit)
		return FALSE;

	// source item grid 
	m_pstMakeSourceItem = m_pAgcmUIManager2->AddUserData("COMPOSE_UD_MAKE_GRID", &m_GridSourceItem, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
	if (NULL == m_pstMakeSourceItem)
		return FALSE;

	// comose button
	m_pstActiveComposeButton = m_pAgcmUIManager2->AddUserData("COMPOSE_UD_COMPOSE_BTN", &m_lComposeBtn, sizeof(INT32),
														  1, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstActiveComposeButton)
		return FALSE;														  

	return TRUE;
	}


BOOL AgcmUIProduct::AddBoolean()
	{
	if (!m_pAgcmUIManager2->AddBoolean(this, "COMPOSE_ACTIVE_COMPOSE_BTN", CBIsActiveComposeButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
	}




//	Open
//========================================================
//
BOOL AgcmUIProduct::Open(INT32 eCategory, INT32 lSkillID)
	{
	//if (m_bIsWindowOpen)
	//	return FALSE;
	
	// initialize
	m_eCategory = eCategory;
	m_lSkillID = lSkillID;
	ZeroMemory(m_lComposeList, sizeof(INT32) * AGCMUIPRODUCT_MAX_LIST);
	m_lTotalCompose = m_pAgpmProduct->GetComposeList(m_pAgcmCharacter->GetSelfCharacter(), eCategory,
													 m_lComposeList, AGCMUIPRODUCT_MAX_LIST);

	// open window
	m_bIsWindowOpen	= TRUE;
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIPRODUCT_EVENT_OPEN]);

	return TRUE;
	}




//	Function Callbacks
//=========================================================
//
BOOL AgcmUIProduct::CBOpenWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProduct	*pThis = (AgcmUIProduct *) pClass;

	// reset
	pThis->m_lSelectedComposeID = 0;
	pThis->m_bHold = FALSE;
	pThis->ClearGrid();

	// set list count
	pThis->m_pstSelectList->m_stUserData.m_lCount = pThis->m_lTotalCompose;

	// refresh
	pThis->RefreshAll();

	return TRUE;
	}


BOOL AgcmUIProduct::CBOpenTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIProduct	*pThis = (AgcmUIProduct *)	pClass;
	INT32			lX	=	0;
	INT32			lY	=	0;

	pcsSourceControl->m_pcsBase->ClientToScreen(&lX, &lY);

	lX += pcsSourceControl->m_pcsBase->w;
	lY += pcsSourceControl->m_pcsBase->h;

	pThis->m_csTooltip.MoveWindow(lX, lY, pThis->m_csTooltip.w, pThis->m_csTooltip.h);

	AgpdComposeTemplate *pAgpdComposeTemplate = pThis->GetComposeByIndex(pcsSourceControl->m_lUserDataIndex);
	if (!pAgpdComposeTemplate)
		return FALSE;

	if (pThis->m_pAgcmUIProductSkill)
		{
		pThis->m_pAgcmUIProductSkill->SetTooltipText(&(pThis->m_csTooltip), pAgpdComposeTemplate, TRUE);
		pThis->m_csTooltip.ShowWindow(TRUE);
		}

	return TRUE;
	}


BOOL AgcmUIProduct::CBCloseTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIProduct	*pThis = (AgcmUIProduct *)	pClass;

	pThis->m_csTooltip.ShowWindow(FALSE);
	pThis->m_csTooltip.DeleteAllStringInfo();

	return TRUE;
	}


BOOL AgcmUIProduct::CBClickListItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIProduct	*pThis = (AgcmUIProduct*)pClass;

	if (pThis->m_bHold)
		return FALSE;

	AgpdComposeTemplate *pAgpdComposeTemplate = pThis->GetComposeByIndex(pcsSourceControl->m_lUserDataIndex);
	if (!pAgpdComposeTemplate)
		return FALSE;

	pThis->m_lSelectedComposeID = pAgpdComposeTemplate->m_lComposeID;
	if (pThis->IsComposable(pAgpdComposeTemplate))
		{
		pThis->m_csTooltip.ShowWindow(FALSE);
		pThis->m_csTooltip.DeleteAllStringInfo();
		}
	else
		{
		pThis->m_lSelectedComposeID = 0;
		}

	pThis->RefreshAll();

	return TRUE;
	}


BOOL AgcmUIProduct::CBClickOkButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIProduct *pThis = (AgcmUIProduct *)	pClass;

	return pThis->OnMake();
	}


BOOL AgcmUIProduct::CBCloseWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIProduct	*pThis = (AgcmUIProduct *)	pClass;
	
	if (pThis->m_bHold)
		return FALSE;
		
	pThis->m_bIsWindowOpen = FALSE;
	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIPRODUCT_EVENT_CLOSE]);
	
	return TRUE;	
	}




//	Display callbacks
//====================================================
//
BOOL AgcmUIProduct::CBDisplayTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIProduct	*pThis = (AgcmUIProduct *) pClass;

	if (pThis->m_eCategory > AGPMPRODUCT_CATEGORY_HUNT && pThis->m_eCategory < AGPMPRODUCT_CATEGORY_MAX)
		strcpy(szDisplay, s_szL10NCategory[pThis->m_eCategory]);

	return TRUE;	
	}


BOOL AgcmUIProduct::CBDisplaySelectEdit(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || AGCDUI_USERDATA_TYPE_INT32 != eType || !pcsSourceControl)
		return FALSE;

	AgcmUIProduct	*pThis = (AgcmUIProduct *) pClass;

	if (AcUIBase::TYPE_EDIT != pcsSourceControl->m_lType || !pcsSourceControl->m_pcsBase)
		return FALSE;

	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetLineDelimiter(pThis->m_pAgcmUIManager2->GetLineDelimiter());

	CHAR szBuffer[512] = {0,};
	pThis->GetMessageTxt(pThis->m_eCategory, AGCMUIPRODUCT_MESSAGE_SELECT_EDIT, szBuffer, sizeof(szBuffer));

	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(szBuffer);

	return TRUE;
	}


BOOL AgcmUIProduct::CBDisplayMakeOk(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIProduct	*pThis = (AgcmUIProduct *) pClass;

	pThis->GetMessageTxt(pThis->m_eCategory, AGCMUIPRODUCT_MESSAGE_OK, szDisplay, 20);

	return TRUE;	
	}


BOOL AgcmUIProduct::CBDisplayMakeCancel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIProduct	*pThis = (AgcmUIProduct *) pClass;

	pThis->GetMessageTxt(pThis->m_eCategory, AGCMUIPRODUCT_MESSAGE_CANCEL, szDisplay, 20);

	return TRUE;	
	}


BOOL AgcmUIProduct::CBDisplaySelectList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pData || !pClass || AGCDUI_USERDATA_TYPE_INT32 != eType)
		return FALSE;

	AgcmUIProduct *pThis = (AgcmUIProduct *) pClass;
	AgpdComposeTemplate *pAgpdComposeTemplate = pThis->GetComposeByIndex(pcsSourceControl->m_lUserDataIndex);

	if (pAgpdComposeTemplate)
		{
		sprintf(szDisplay, "%s", pAgpdComposeTemplate->m_ResultItem.m_pItemTemplate->m_szName);

		if (pThis->IsComposable(pAgpdComposeTemplate))
			pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor = COLOR_WHITE;
		else
			pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor = COLOR_GRAY;
		}
	else
		szDisplay[0] = _T('\0');
	
	return TRUE;	
	}


BOOL AgcmUIProduct::CBDisplayMakeEdit(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || AGCDUI_USERDATA_TYPE_INT32 != eType || !pcsSourceControl)
		return FALSE;

	AgcmUIProduct	*pThis = (AgcmUIProduct *) pClass;

	if (AcUIBase::TYPE_EDIT != pcsSourceControl->m_lType || !pcsSourceControl->m_pcsBase)
		return FALSE;

	CHAR szBuffer[512] = {0,};
	AgpdComposeTemplate *pAgpdComposeTemplate = pThis->m_pAgpmProduct->GetComposeTemplate(pThis->m_lSelectedComposeID);
	if (!pAgpdComposeTemplate)
		{
		((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(szBuffer);
		return FALSE;
		}

	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetLineDelimiter(pThis->m_pAgcmUIManager2->GetLineDelimiter());
	
	// name
	sprintf(szBuffer + strlen(szBuffer), "______"
										"[%s]______" 
										"- %s -___",
										pAgpdComposeTemplate->m_ResultItem.m_pItemTemplate->m_szName, ClientStr().GetStr(STI_NEED_MATERIAL));

	// source items
	for (int i = 0; i < pAgpdComposeTemplate->m_lRequiredItems; ++i)
		{
		sprintf(szBuffer + strlen(szBuffer), "%s %d%s___", pAgpdComposeTemplate->m_RequiredItems[i].m_pItemTemplate->m_szName,
														   pAgpdComposeTemplate->m_RequiredItems[i].m_lCount, ClientStr().GetStr(STI_NUMBER_UNIT));
		}

	// receipe
	if (pAgpdComposeTemplate->m_Receipe.m_pItemTemplate)
		sprintf(szBuffer + strlen(szBuffer), ClientStr().GetStr(STI_UP_RATE), pAgpdComposeTemplate->m_Receipe.m_pItemTemplate->m_szName);

	// level
	if (pThis->m_eCategory > AGPMPRODUCT_CATEGORY_HUNT && pThis->m_eCategory < AGPMPRODUCT_CATEGORY_MAX)
		{
		sprintf(szBuffer + strlen(szBuffer), ClientStr().GetStr(STI_AUTO_USE), s_szL10NCategory[pThis->m_eCategory]);
		}

	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(szBuffer);

	return TRUE;
	}





//	Cooldown Callback
//======================================================
//
BOOL AgcmUIProduct::CBAfterCooldown(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;
		
	AgcmUIProduct *pThis = (AgcmUIProduct *) pClass;
	INT32 lCooldownID = (INT32) pData;
	
	pThis->m_bHold = FALSE;
	if (pThis->m_lCooldownID != lCooldownID)
		{
		TRACE("!!!Error : cooldown id not matched in AgcmUIProduct::CBAfterCooldown()\n");
		return FALSE;
		}
	
	return pThis->m_pAgcmProduct->SendPacketCompose(pThis->m_pAgcmCharacter->GetSelfCharacter()->m_lID, 
													pThis->m_lSkillID,
													pThis->m_lSelectedComposeID,
													pThis->m_lReceipeID
													);	
	}




//	Operation
//============================================
//
BOOL AgcmUIProduct::OnMake()
	{
	if (m_bHold)
		return FALSE;
	
	AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
	if (!pAgpdCharacter)
		return FALSE;

	AgpmProduct* ppmProduct = ( AgpmProduct* )g_pEngine->GetModule( "AgpmProduct" );
	AgcmTitle* pcmTitle = GetAgcmTitle();
	if( !ppmProduct || !pcmTitle ) return FALSE;

	int nCurrTitleID = pcmTitle->GetCurrentActivateTitleID();
	if( !ppmProduct->IsActiveRequireTitle( m_lSelectedComposeID, nCurrTitleID ) )
	{
		char* pErrorMsg = m_pAgcmUIManager2->GetUIMessage( "NotActivateRequireTitle" );
		if( pErrorMsg && strlen( pErrorMsg ) > 0 )
		{
			m_pAgcmUIManager2->ActionMessageOKDialog( pErrorMsg );
		}

		return FALSE;
	}
	
	m_bHold = TRUE;
	
	// sound
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIPRODUCT_EVENT_MAKE]);

	// cast animation
	FLOAT	fAnimDuration	= 0.0f;
	CastAnimation(pAgpdCharacter, &fAnimDuration);

	if (fAnimDuration == 0.0f)
		fAnimDuration = 3.0f;
	
	if (m_pAgcmUICooldown && AGCMUICOOLDOWN_INVALID_ID != m_lCooldownID)
	{
		m_pAgpmCharacter->SetActionBlockTime(pAgpdCharacter, (UINT32) (fAnimDuration * 1000), AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);

		return m_pAgcmUICooldown->StartCooldown(m_lCooldownID, (UINT32) fAnimDuration * 1000);
	}
	else
		m_pAgcmProduct->SendPacketCompose(pAgpdCharacter->m_lID, m_lSkillID, m_lSelectedComposeID, m_lReceipeID);

	return TRUE;	
	}


BOOL AgcmUIProduct::CastAnimation(AgpdCharacter *pAgpdCharacter, FLOAT *pfAnimDuration)
	{
	AgcdCharacter *pAgcdCharacter = m_pAgcmCharacter->GetCharacterData(pAgpdCharacter);
	
	AgcdSkillAttachTemplateData	*pAgcdSkillTAD = 
				m_pAgcmSkill->GetAttachTemplateData(pAgpdCharacter->m_pcsCharacterTemplate);
	if (!pAgcdSkillTAD)
		return FALSE;
	
	AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkill(m_lSkillID);
	if (!pAgpdSkill || !pAgpdSkill->m_pcsTemplate)
		return FALSE;
	
	INT32 lIndex = m_pAgcmSkill->GetUsableSkillTNameIndex(pAgpdCharacter->m_pcsCharacterTemplate,
														  ((AgpdSkillTemplate *) pAgpdSkill->m_pcsTemplate)->m_lID
														  );
	if (0 > lIndex)
		return FALSE;

	m_pAgcmSkill->SetCurrentSkillUsableIndex(pAgpdCharacter, lIndex);

	if (pAgcdSkillTAD->m_pacsSkillVisualInfo[lIndex]->m_pacsAnimation[pAgcdCharacter->m_lCurAnimType2])
		{
		*pfAnimDuration	=
			m_pAgcmCharacter->GetAnimDuration(
				pAgcdSkillTAD->m_pacsSkillVisualInfo[lIndex]->m_pacsAnimation[pAgcdCharacter->m_lCurAnimType2]->m_pcsAnimation	);

		m_pAgcmCharacter->StartAnimation(pAgpdCharacter, AGCMCHAR_ANIM_TYPE_SKILL,
				pAgcdSkillTAD->m_pacsSkillVisualInfo[lIndex]->m_pacsAnimation[pAgcdCharacter->m_lCurAnimType2]
				);
		}
	else if (pAgcdSkillTAD->m_pacsSkillVisualInfo[lIndex]->m_pacsAnimation[AGCMCHAR_AT2_COMMON_DEFAULT])
		{
		*pfAnimDuration	=
			m_pAgcmCharacter->GetAnimDuration(
				pAgcdSkillTAD->m_pacsSkillVisualInfo[lIndex]->m_pacsAnimation[AGCMCHAR_AT2_COMMON_DEFAULT]->m_pcsAnimation	);

		m_pAgcmCharacter->StartAnimation(pAgpdCharacter, AGCMCHAR_ANIM_TYPE_SKILL,
				pAgcdSkillTAD->m_pacsSkillVisualInfo[lIndex]->m_pacsAnimation[AGCMCHAR_AT2_COMMON_DEFAULT]
				);
		}
	
	return TRUE;
	}




//	Result
//==========================================================
//
BOOL AgcmUIProduct::OnResult(AgpdSkill *pAgpdSkill, INT32 lResult, INT32 lItemTID, INT32 lExp, int nCount )
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
			sprintf(sz, "%s%s", pszSkillName, ClientStr().GetStr(STI_SKILL_SUCCESS));
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
				sprintf(sz, "%s%s %d %s", pszSkillName, ClientStr().GetStr(STI_SKILLMASTERY), lExp, ClientStr().GetStr(STI_MASTERY_UP));
				AddSystemMessage(sz);
				}
				
			// sound
			m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIPRODUCT_EVENT_SUCCESS]);
			}
			break;
			
		case AGPMPRODUCT_RESULT_FAIL:
			{
			// fail
			sprintf(sz, "%s%s", pszSkillName, ClientStr().GetStr(STI_SKILL_FAIL));
			AddSystemMessage(sz);
			
			// sound
			m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIPRODUCT_EVENT_FAIL]);
			}
			break;
			
		case AGPMPRODUCT_RESULT_FAIL_NO_TOOL:
		{
			AgpdItemTemplate *pAgpdItemTemplate = m_pAgpmItem->GetItemTemplate(
													pAgpdSkillTemplate->m_stConditionArg[0].lArg4);
			if (pAgpdItemTemplate)
			{
				if (AP_SERVICE_AREA_WESTERN == g_eServiceArea)
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
			break;
			
		case AGPMPRODUCT_RESULT_FAIL_INSUFFICIENT_SOURCE_ITEM:
			{
			AgpdItemTemplate *pAgpdItemTemplate = m_pAgpmItem->GetItemTemplate(lItemTID);
			if (pAgpdItemTemplate)
				{
				sprintf(sz, "%s %s", pAgpdItemTemplate->m_szName, ClientStr().GetStr(STI_NO_MATERIAL));
				AddSystemMessage(sz);
				}	
			}
			break;
			
		case AGPMPRODUCT_RESULT_FAIL_NO_REMAINED_ITEM:
			{
			sprintf(sz, ClientStr().GetStr(STI_NO_GET));
			AddSystemMessage(sz);
			}
			break;
			
		default :
			break;
		}
	
	OnPostResult();
	return TRUE;
	}




void AgcmUIProduct::OnPostResult()
	{
	m_bHold			= FALSE;
	m_lReceipeID	= 0;


	AgpdComposeTemplate* pAgpdComposeTemplate = m_pAgpmProduct->GetComposeTemplate(m_lSelectedComposeID);
	if (!IsComposable(pAgpdComposeTemplate))
		m_lSelectedComposeID = 0;

	RefreshAll();
	}




//	Display helper
//==========================================================
//
BOOL AgcmUIProduct::DisplaySourceItem()
	{
	ClearGrid();
	
	AgpdComposeTemplate* pAgpdComposeTemplate = m_pAgpmProduct->GetComposeTemplate(m_lSelectedComposeID);
	if (!pAgpdComposeTemplate)
		return FALSE;

	BOOL bResult = TRUE;
	INT32 i;
	for (i = 0; i < pAgpdComposeTemplate->m_lRequiredItems; ++i)
		{
		if (!m_pAgpmProduct->IsValidItemStatus(m_pAgcmCharacter->GetSelfCharacter(), pAgpdComposeTemplate->m_RequiredItems[i].m_pItemTemplate,
					pAgpdComposeTemplate->m_RequiredItems[i].m_lCount))
			{
			bResult = FALSE;
			break;
			}
		}

	if (bResult && pAgpdComposeTemplate->m_lRequiredItems > 0)
		{
		for (i = 0; i < pAgpdComposeTemplate->m_lRequiredItems; ++i)
			{
			AddGridItem(pAgpdComposeTemplate->m_RequiredItems[i].m_pItemTemplate,
						pAgpdComposeTemplate->m_RequiredItems[i].m_lCount,
						i);
			}

		if (NULL != pAgpdComposeTemplate->m_Receipe.m_pItemTemplate)		// receipe item
			{
			INT32	lIndex = 0;
			AgpdGridItem *pAgpdGridItem;
			AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(m_pAgcmCharacter->GetSelfCharacter());
			for (pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex);
				 pAgpdGridItem;
				 pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex))
				{
				// Check TID
				if (pAgpdComposeTemplate->m_Receipe.m_pItemTemplate->m_lID != pAgpdGridItem->m_lItemTID)
					continue;

				// Stackable
				AgpdItem* pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem);
				if (pAgpdItem)
					{
					m_lReceipeID = pAgpdItem->m_lID;
					AddGridItem(pAgpdComposeTemplate->m_Receipe.m_pItemTemplate, 1, i);
					break;
					}
				}
			}
		}

	RefreshSourceItemGrid();
	return TRUE;
	}


BOOL AgcmUIProduct::AddGridItem(AgpdItemTemplate *pItemTemplate, INT32 lCount, INT32 lIndex)
	{
	m_pGridItemList[lIndex] = m_pAgpmGrid->CreateGridItem();
	
	SetGridItemAttachedTexture(m_pGridItemList[lIndex], pItemTemplate);
	
	if (pItemTemplate->m_bStackable)
		{
		CHAR szRightString[AGPDGRIDITEM_BOTTOM_STRING_LENGTH + 1] = {0,};
		sprintf(szRightString, "%d", lCount);
		m_pGridItemList[lIndex]->SetRightBottomString(szRightString);
		}

	m_pAgpmGrid->AddItem(&m_GridSourceItem, m_pGridItemList[lIndex]);
	m_pAgpmGrid->Add(&m_GridSourceItem, m_pGridItemList[lIndex], 1, 1);

	return TRUE;
	}


BOOL AgcmUIProduct::SetGridItemAttachedTexture(AgpdGridItem *pAgpdGridItem, AgpdItemTemplate *pAgpdItemTempalte)
	{
	if (!pAgpdGridItem)
		return FALSE;

	RwTexture **ppRwTexture = (RwTexture **)(m_pAgpmGrid->GetAttachedModuleData( 
			m_pAgcmUIControl->m_lItemGridTextureADDataIndex, pAgpdGridItem ));

	AgcdItemTemplate *pAgcdItemTemplate = m_pAgcmItem->GetTemplateData(pAgpdItemTempalte);
		
	(RwTexture *) *ppRwTexture = pAgcdItemTemplate->m_pTexture;

	return TRUE;		
	}


BOOL AgcmUIProduct::RefreshSourceItemGrid()
	{
	if (m_pstMakeSourceItem)
		{
		m_pstMakeSourceItem->m_stUserData.m_pvData	= &m_GridSourceItem;
		return m_pAgcmUIManager2->SetUserDataRefresh(m_pstMakeSourceItem);
		}

	return TRUE;
	}


BOOL AgcmUIProduct::ClearGrid()
	{
	m_pAgpmGrid->Reset(&m_GridSourceItem);
	RefreshSourceItemGrid();

	for (int i = 0; i < AGCMUIPRODUCT_MAX_GRID; ++i)
		{
		if (!m_pGridItemList[i]) 
			break;

		m_pAgpmGrid->DeleteGridItem(m_pGridItemList[i]);
		}

	ZeroMemory(m_pGridItemList, sizeof(AgpdGridItem*) * AGCMUIPRODUCT_MAX_GRID);

	return TRUE;
	}




//	Drag and drop callback
//==========================================================
//
BOOL AgcmUIProduct::CBMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIProduct	*pThis = (AgcmUIProduct *)	pClass;
	AgcdUIControl	*pcsControl = pcsSourceControl;

	if (!pcsControl ||
		!pcsControl->m_pcsBase ||
		AcUIBase::TYPE_GRID != pcsControl->m_lType)
		return FALSE;

	AcUIGrid				*pcsUIGrid			= (AcUIGrid *) pcsControl->m_pcsBase;
	AgpdGridSelectInfo		*pstGridSelectInfo	= pcsUIGrid->GetDragDropMessageInfo();
	if (!pstGridSelectInfo || !pstGridSelectInfo->pGridItem || AGPDGRID_ITEM_TYPE_ITEM != pstGridSelectInfo->pGridItem->m_eType)
		return FALSE;

	AgpdGrid *pcsControlGrid = pThis->m_pAgcmUIManager2->GetControlGrid(pcsControl);
	if (!pcsControlGrid)
		return FALSE;

	AgpdItem *pAgpdItem = pThis->m_pAgpmItem->GetItem(pstGridSelectInfo->pGridItem->m_lItemID);
	if (!pAgpdItem)
		return FALSE;

	// it must be in inventory
	if (AGPDITEM_STATUS_INVENTORY == pAgpdItem->m_eStatus)
		{
		// ignore when dropped item isn't required receipe item
		if (((AgpdItemTemplate *) pAgpdItem->m_pcsItemTemplate)->m_lID != 
			pThis->m_pAgpmProduct->GetComposeTemplate(pThis->m_lSelectedComposeID)->m_Receipe.m_pItemTemplate->m_lID )
			return FALSE;

		// it must usable and receipe
		if (AGPMITEM_TYPE_USABLE != ((AgpdItemTemplate *) pAgpdItem->m_pcsItemTemplate)->m_nType ||
			AGPMITEM_USABLE_TYPE_RECEIPE != ((AgpdItemTemplateUsable *) pAgpdItem->m_pcsItemTemplate)->m_nUsableItemType)
			return FALSE;

		pThis->m_lReceipeID = pAgpdItem->m_lID;
		pThis->m_pAgpmGrid->AddItem(&pThis->m_GridSourceItem, pAgpdItem->m_pcsGridItem);
		pThis->m_pAgpmGrid->Add(&pThis->m_GridSourceItem, pAgpdItem->m_pcsGridItem, 1, 1);
		pThis->RefreshSourceItemGrid();
		}

	return TRUE;
	}




//	Boolean callback
//====================================================
//
BOOL AgcmUIProduct::CBIsActiveComposeButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProduct *pThis	= (AgcmUIProduct *)	pClass;

	if (!pThis->m_pAgcmCharacter->GetSelfCharacter() || 0 == pThis->m_lSkillID)
		return FALSE;

	 if (0 == pThis->m_lSelectedComposeID)
		return FALSE;

	// validate
	AgpdSkill *pAgpdSkill = pThis->m_pAgpmSkill->GetSkill(pThis->m_lSkillID);
	AgpdComposeTemplate *pAgpdComposeTemplate = pThis->m_pAgpmProduct->GetComposeTemplate(pThis->m_lSelectedComposeID);
	INT32 lResult = AGPMPRODUCT_RESULT_FAIL;
	if (!pThis->m_pAgpmProduct->IsValidStatus(pThis->m_pAgcmCharacter->GetSelfCharacter(), pAgpdComposeTemplate, pAgpdSkill, &lResult))
		{
		return FALSE;
		}

	return TRUE;
	}




//	System driven UI close callback
//==================================================
//
BOOL AgcmUIProduct::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProduct *pThis = (AgcmUIProduct *) pClass;
	
	pThis->m_bIsWindowOpen = FALSE;
	pThis->m_bHold = FALSE;

	return TRUE;
	}


BOOL AgcmUIProduct::CBCloseAllUIToolTip(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUIProduct *pThis = (AgcmUIProduct *) pClass;
	pThis->m_csTooltip.DeleteAllStringInfo();

	return TRUE;
	}


void AgcmUIProduct::RefreshAll()
	{
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstDummy);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstSelectList);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstMakeEdit);
	DisplaySourceItem();
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstActiveComposeButton);
	}


//	Helper
//=======================================================
//
BOOL AgcmUIProduct::IsComposable(AgpdComposeTemplate *pAgpdComposeTemplate)
	{
	if (!pAgpdComposeTemplate)
		return FALSE;
	
	AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkill(m_lSkillID);
	return m_pAgpmProduct->IsValidStatus(m_pAgcmCharacter->GetSelfCharacter(), pAgpdComposeTemplate, pAgpdSkill);
	}


AgpdComposeTemplate* AgcmUIProduct::GetComposeByIndex(INT32 lUserDataIndex)
	{
	INT32 lIndex = lUserDataIndex;
	return m_pAgpmProduct->GetComposeTemplate(m_lComposeList[lIndex]);
	}


INT32 AgcmUIProduct::GetMessageTxt(INT32 eCategory, eAGCMUIPRODUCT_MESSAGE eMessage, CHAR *pszBuffer, INT32 lSize)
	{
	switch (eMessage)
		{
		case AGCMUIPRODUCT_MESSAGE_TITLE :
			strcpy(pszBuffer, ClientStr().GetStr(STI_TITLE));
			break;

		case AGCMUIPRODUCT_MESSAGE_SELECT_EDIT :
			{
			switch (eCategory)
				{
				case AGPMPRODUCT_CATEGORY_COOK:
					sprintf(pszBuffer, ClientStr().GetStr(STI_COOK_DESC),
									   s_szL10NCategory[eCategory]
									   );
									  

					break;
				case AGPMPRODUCT_CATEGORY_ALCHEMY:
					sprintf(pszBuffer, ClientStr().GetStr(STI_ALCHEMY_DESC),
									   s_szL10NCategory[eCategory]
									   );
					break;
				}
			}
	
			break;

		case AGCMUIPRODUCT_MESSAGE_MAKE_TITLE :
			strcpy(pszBuffer, ClientStr().GetStr(STI_MAKE_IT));
			break;

		case AGCMUIPRODUCT_MESSAGE_MAKE_NAME :
			strcpy(pszBuffer, "");
			break;

		case AGCMUIPRODUCT_MESSAGE_MAKE_LEVEL :
			strcpy(pszBuffer, "");
			break;

		case AGCMUIPRODUCT_MESSAGE_MAKE_PRICE :
			strcpy(pszBuffer, "");
			break;

		case AGCMUIPRODUCT_MESSAGE_OK :
			strcpy(pszBuffer, ClientStr().GetStr(STI_MAKE));
			break;

		case AGCMUIPRODUCT_MESSAGE_CANCEL :
			strcpy(pszBuffer, ClientStr().GetStr(STI_CLOSE));
			break;

		default :
			strcpy(pszBuffer, "");
			break;
			break;
		}

	
	return 0;	
	}


CHAR* AgcmUIProduct::GetMessageTxt(INT32 eCategory, eAGCMUIPRODUCT_MESSAGE eMessage)
	{
	if (!m_pAgcmUIManager2)
		return NULL;

	CHAR szKey[64];
	CHAR szPost[16];

	switch (eCategory)
		{
		case AGPMPRODUCT_CATEGORY_COOK :
			strcpy(szPost, _T("_COOK"));
			break;

		case AGPMPRODUCT_CATEGORY_ALCHEMY:
			strcpy(szPost, _T("_ALCHEMY"));
			break;

		default :
			strcpy(szPost, _T(""));
			break;
		}

	strcpy(szKey, s_szMessage[eMessage]);
	strcat(szKey, szPost);

	return m_pAgcmUIManager2->GetUIMessage(szKey);
	}


BOOL AgcmUIProduct::AddSystemMessage(CHAR* pszMsg)
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

