/*===========================================================================

	AgcmUIGamble.cpp

===========================================================================*/


#include "AgcmUIGamble.h"
#include "AuStrTable.h"


/********************************************************/
/*		The Implementation of AgcmUIGamble class		*/
/********************************************************/
//
CHAR AgcmUIGamble::s_szMessage[AGCMUIGAMBLE_MESSAGE_MAX][30] = 
	{
	"GAMBLE_SUCCESS",			// AGCMUIGAMBLE_MESSAGE_SUCCESS = 0,
	"GAMBLE_FULL_INVEN",		// AGCMUIGAMBLE_MESSAGE_FULL_INVEN,
	"GAMBLE_NA_RACE",			// AGCMUIGAMBLE_MESSAGE_NA_RACE,
	"GAMBLE_NA_CLASS",			// AGCMUIGAMBLE_MESSAGE_NA_CLASS,
	"GAMBLE_NE_LEVEL",			// AGCMUIGAMBLE_MESSAGE_NOT_ENOUGH_LEVEL,
	"GAMBLE_NE_POINT",			// AGCMUIGAMBLE_MESSAGE_NOT_ENOUGH_POINT,
	"GAMBLE_CONFIRM",			// AGCMUIGAMBLE_MESSAGE_CONFIRM,
	};							// AGCMUIGAMBLE_MESSAGE_MAX


CHAR AgcmUIGamble::s_szEvent[AGCMUIGAMBLE_EVENT_MAX][30] =
	{
	"GAMBLE_OPEN",		//	AGCMUIGAMBLE_EVENT_OPEN
	"GAMBLE_CLOSE"		//	AGCMUIGAMBLE_EVENT_CLOSE
	};


AgcmUIGamble::AgcmUIGamble()
	{
	SetModuleName("AgcmUIGamble");
	}


AgcmUIGamble::~AgcmUIGamble()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgcmUIGamble::OnAddModule()
	{
	m_pAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pAgpmGrid = (AgpmGrid *) GetModule("AgpmGrid");
	m_pAgpmGamble = (AgpmGamble *) GetModule("AgpmGamble");
	
	m_pAgcmCharacter = (AgcmCharacter *) GetModule("AgcmCharacter");
	m_pAgcmItem = (AgcmItem *) GetModule("AgcmItem");
	m_pAgcmUIManager2 = (AgcmUIManager2 *) GetModule("AgcmUIManager2");
	m_pAgcmUIControl = (AgcmUIControl *) GetModule("AgcmUIControl");
	m_pAgcmUIMain = (AgcmUIMain *) GetModule("AgcmUIMain");
	m_pAgcmEventManager	= (AgcmEventManager *) GetModule("AgcmEventManager");
	m_pAgcmChatting2 = (AgcmChatting2 *) GetModule("AgcmChatting2");
	m_pAgcmUICharacter = (AgcmUICharacter *) GetModule("AgcmUICharacter");

	if (!m_pAgpmCharacter || !m_pAgpmGamble || !m_pAgpmItem || !m_pAgpmGrid
		|| !m_pAgcmCharacter || !m_pAgcmItem || !m_pAgcmUIManager2 || !m_pAgcmUIControl || !m_pAgcmUIMain
		|| !m_pAgcmEventManager || !m_pAgcmChatting2 || !m_pAgcmUICharacter
		)
		return FALSE;	

	// module
	if (FALSE == m_pAgpmGamble->SetCallbackGamble(CBResultGamble, this))
		return FALSE;
	if (FALSE == m_pAgpmCharacter->SetCallbackUpdateLevel(CBUpdateLevel, this))
		return FALSE;
	if (FALSE == m_pAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;
	if (FALSE == m_pAgcmCharacter->SetCallbackReleaseSelfCharacter(CBReleaseSelfCharacter, this))
		return FALSE;
	if (FALSE == m_pAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	// event
	if (FALSE == m_pAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_GAMBLE, CBRequestEvent, this))
		return FALSE;	
	if (FALSE == m_pAgpmGamble->SetCallbackEventGrant(CBGrant, this))
		return FALSE;

	// Main UI callback
	if (FALSE == m_pAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	// UI
	if (!AddEvent() || !AddFunction() || !AddDisplay() || !AddUserData())
		return FALSE;
	
	return TRUE;
	}


BOOL AgcmUIGamble::OnInit()
	{
	// initialize grid(only grid, not grid item)
	m_pAgpmGrid->Init(&m_AgpdGrid_TemplateList, AGCMUIGAMBLE_GRID_LAYER, AGCMUIGAMBLE_GRID_ROW, AGCMUIGAMBLE_GRID_COLUMN);
	m_AgpdGrid_TemplateList.m_lGridType = AGPDGRID_ITEM_TYPE_ITEM;
	m_pAgcdUIUserData_Grid->m_stUserData.m_pvData = &m_AgpdGrid_TemplateList;
	m_pAgcdUIUserData_Grid->m_stUserData.m_lCount = 0;

	// initialize tooltip
	m_Tooltip.m_Property.bTopmost = TRUE;
	m_pAgcmUIManager2->AddWindow((AgcWindow *) (&m_Tooltip));
	m_Tooltip.ShowWindow(FALSE);
	
  #ifdef _DEBUG
	AS_REGISTER_TYPE_BEGIN(AgcmUIGamble, AgcmUIGamble);
		AS_REGISTER_METHOD0(void, TestOpen);
	AS_REGISTER_TYPE_END;
  #endif
  
	return TRUE;
	}


BOOL AgcmUIGamble::OnDestroy()
	{
	// remove grid	
	m_pAgpmGrid->Remove(&m_AgpdGrid_TemplateList);
	
	return TRUE;
	}




//	OnAddModule helper
//=================================================================
//
BOOL AgcmUIGamble::AddEvent()
	{
	for (int i=0; i < AGCMUIGAMBLE_EVENT_MAX; ++i)
		{
		m_lEvent[i] = m_pAgcmUIManager2->AddEvent(s_szEvent[i]);
		if (m_lEvent[i] < 0)
			return FALSE;
		}

	return TRUE;	
	}


BOOL AgcmUIGamble::AddFunction()
	{
	if (!m_pAgcmUIManager2->AddFunction(this, "GAMBLE_CLOSE", CBClickClose, 0))
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, "GAMBLE_GRID_SET_FOCUS", CBGridSetFocus, 0))
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, "GAMBLE_GRID_KILL_FOCUS", CBGridKillFocus, 0))
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, "GAMBLE_RCLICK_GRID", CBRightClickGrid, 0))
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIGamble::AddDisplay()
	{
	return TRUE;
	}


BOOL AgcmUIGamble::AddUserData()
	{
	m_pAgcdUIUserData_Grid = m_pAgcmUIManager2->AddUserData("GAMBLE_UD_GRID", &m_AgpdGrid_TemplateList, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
	if (NULL == m_pAgcdUIUserData_Grid)
		return FALSE;
	
	return TRUE;
	}




//	UI
//===========================================================
//
BOOL AgcmUIGamble::Open()
	{
	AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return FALSE;

	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIGAMBLE_EVENT_OPEN]);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pAgcdUIUserData_Grid);
	
	return TRUE;
	}


void AgcmUIGamble::Close()
	{
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIGAMBLE_EVENT_CLOSE]);
	}


#ifdef _DEBUG

void AgcmUIGamble::TestOpen()
	{
	Open();
	}

#endif




//	Packet Send
//============================================
//
BOOL AgcmUIGamble::SendRequestEvent(AgpdCharacter *pAgpdCharacter, ApdEvent *pApdEvent)
	{
	if (NULL == pAgpdCharacter || NULL == pApdEvent)
		return FALSE;

	INT16 nPacketLength	= 0;
	PVOID pvPacket = m_pAgpmGamble->MakePacketEventRequest(pApdEvent, pAgpdCharacter->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmGamble->m_csPacket.FreePacket(pvPacket);

	return	bResult;
	}


BOOL AgcmUIGamble::SendGamble(AgpdCharacter *pAgpdCharacter, INT32 lGambleTID)
	{
	if (NULL == pAgpdCharacter || 0 == lGambleTID)
		return FALSE;

	INT8 cOperation = AGPMGAMBLE_OPERATION_GAMBLE;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = NULL;

	PVOID pvPacket = m_pAgpmGamble->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMGAMBLE_PACKET_TYPE,
														  &cOperation,
														  &pAgpdCharacter->m_lID,
														  &lGambleTID,
														  NULL,
														  pvPacketEmb
														  );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmGamble->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}




//	Module Callback
//============================================
//
BOOL AgcmUIGamble::CBResultGamble(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUIGamble *pThis = (AgcmUIGamble *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdGambleParam *pParam = (AgpdGambleParam *) pCustData;

	AgpdCharacter *pAgpdCharacterSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacterSelf || pAgpdCharacter != pAgpdCharacterSelf)
		return FALSE;

	return pThis->OnResultGamble(pParam->m_lResult, pParam->m_lTID);
	}


BOOL AgcmUIGamble::CBUpdateLevel(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgcmUIGamble *pThis = (AgcmUIGamble *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	
	if (pThis->m_pAgcmCharacter->GetSelfCharacter() != pAgpdCharacter)		// self character
		return TRUE;

	pThis->Close();
	return pThis->PrepareGambleGridItem(pAgpdCharacter);
	}


BOOL AgcmUIGamble::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgcmUIGamble *pThis = (AgcmUIGamble *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;

	return pThis->PrepareGambleGridItem(pAgpdCharacter);
	}


BOOL AgcmUIGamble::CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgcmUIGamble *pThis = (AgcmUIGamble *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;

	pThis->RemoveGambleGridItem();
	
	return TRUE;
	}


BOOL AgcmUIGamble::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIGamble	*pThis = (AgcmUIGamble *) pClass;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pData;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pAgpdCharacter->m_stPos, pThis->m_stOpenPos);
	if ((INT32) fDistance < 150)
		return TRUE;

	pThis->Close();
	return TRUE;
	}




//	Event Callback
//============================================
//
BOOL AgcmUIGamble::CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGamble	*pThis			= (AgcmUIGamble *)		pClass;
	ApdEvent		*pApdEvent		= (ApdEvent *)		pData;
	ApBase			*pcsGenerator	= (ApBase *)		pCustData;

	if (pcsGenerator->m_eType == APBASE_TYPE_CHARACTER &&
		pcsGenerator->m_lID	== pThis->m_pAgcmCharacter->GetSelfCID())
		{
		if (pThis->m_pAgpmCharacter->IsActionBlockCondition(pThis->m_pAgcmCharacter->m_pcsSelfCharacter))
			return FALSE;

		return pThis->SendRequestEvent(pThis->m_pAgcmCharacter->m_pcsSelfCharacter, pApdEvent);
		}

	return TRUE;	
	}


BOOL AgcmUIGamble::CBGrant(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pCustData || !pClass)
		return FALSE;

	AgcmUIGamble	*pThis = (AgcmUIGamble *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;
	
	if (pAgpdCharacter != pThis->m_pAgcmCharacter->GetSelfCharacter())
		return FALSE;
	
	// open
	return pThis->Open();
	}




//	Main UI Callback
//================================================
//
BOOL AgcmUIGamble::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGamble *pThis = (AgcmUIGamble *) pClass;

	return TRUE;
	}




//	UI Function Callback
//================================================
//
BOOL AgcmUIGamble::CBClickClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGamble	*pThis = (AgcmUIGamble *) pClass;

	pThis->Close();
	return TRUE;
	}


BOOL AgcmUIGamble::CBGridSetFocus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUIGamble	*pThis = (AgcmUIGamble *) pClass;
	AcUIGrid		*pGrid = (AcUIGrid *) pcsSourceControl->m_pcsBase;

	if (!pGrid->m_pToolTipAgpdGridItem || pGrid->m_pToolTipAgpdGridItem->m_eType != AGPDGRID_ITEM_TYPE_ITEM)
		return FALSE;
	
	// show tooltip
	pThis->OpenTooltip(pGrid->m_pToolTipAgpdGridItem->m_lItemTID, pGrid->m_lItemToolTipX, pGrid->m_lItemToolTipY);

	return TRUE;
	}


BOOL AgcmUIGamble::CBGridKillFocus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGamble	*pThis = (AgcmUIGamble *) pClass;

	pThis->CloseTooltip();

	return TRUE;
	}


BOOL AgcmUIGamble::CBRightClickGrid(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUIGamble	*pThis = (AgcmUIGamble *) pClass;
	AcUIGrid		*pGrid = (AcUIGrid *) pcsSourceControl->m_pcsBase;

	AgpdGridSelectInfo *pSelectInfo	= pGrid->GetGridItemClickInfo();
	if (!pSelectInfo || !pSelectInfo->pGridItem || AGPDGRID_ITEM_TYPE_ITEM != pSelectInfo->pGridItem->m_eType)
		return FALSE;

	// get self character
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (!pAgpdCharacter)
		return FALSE;

	// get gamble template
	INT32 lGambleTID = pSelectInfo->pGridItem->m_lItemTID;
	AgpdGamble *pAgpdGamble = pThis->m_pAgpmGamble->GetTemplate(pSelectInfo->pGridItem->m_lItemTID);
	if (!pAgpdGamble)
		return FALSE;
	
	if (IDOK != pThis->m_pAgcmUIManager2->ActionMessageOKCancelDialog(pThis->GetMessageTxt(AGCMUIGAMBLE_MESSAGE_CONFIRM)))
		return TRUE;

	// validation
	INT32 lResult = AGPMGAMBLE_RESULT_NONE;
	if (!pThis->m_pAgpmGamble->IsValid(pAgpdCharacter, pAgpdGamble, &lResult))
		{
		pThis->OnResultGamble(lResult, 0);
		return TRUE;
		}
	
	// send packet	
	pThis->SendGamble(pAgpdCharacter, lGambleTID);
	
	return TRUE;
	}




//	Helper
//================================================
//
BOOL AgcmUIGamble::OnResultGamble(INT32 lResult, INT32 lTID)
	{
	CHAR *psz = NULL;
	CHAR szMessage[1024];		ZeroMemory(szMessage, sizeof(szMessage));

	switch (lResult)
		{
		case AGPMGAMBLE_RESULT_SUCCESS :
			{
			psz = GetMessageTxt(AGCMUIGAMBLE_MESSAGE_SUCCESS);
			AgpdItemTemplate *pAgpdItemTemplate = m_pAgpmItem->GetItemTemplate(lTID);
			if (pAgpdItemTemplate && psz)
			sprintf(szMessage, psz, pAgpdItemTemplate->m_szName);
			}
			break;
		
		case AGPMGAMBLE_RESULT_NA_RACE :
			psz = GetMessageTxt(AGCMUIGAMBLE_MESSAGE_NA_RACE);
			break;
			
		case AGPMGAMBLE_RESULT_NA_CLASS :
			psz = GetMessageTxt(AGCMUIGAMBLE_MESSAGE_NA_CLASS);
			break;
			
		case AGPMGAMBLE_RESULT_NOT_ENOUGH_LEVEL :
			psz = GetMessageTxt(AGCMUIGAMBLE_MESSAGE_NOT_ENOUGH_LEVEL);
			break;

		case AGPMGAMBLE_RESULT_NOT_ENOUGH_POINT:
			psz = GetMessageTxt(AGCMUIGAMBLE_MESSAGE_NOT_ENOUGH_POINT);
			break;
			
		case AGPMGAMBLE_RESULT_FULL_INVENTORY :
			psz = GetMessageTxt(AGCMUIGAMBLE_MESSAGE_FULL_INVEN);
			break;

		default :
			break;
		}
		
	// non formatted message
	if (psz && !szMessage[0])
		strcpy(szMessage, psz);
	
	if (szMessage[0])
		AddSystemMessage(szMessage);
	
	return TRUE;
	}


BOOL AgcmUIGamble::PrepareGambleGridItem(AgpdCharacter *pAgpdCharacter)
	{
	RemoveGambleGridItem();
	
	// for all gamble template(race, class specific)
	for (GambleMapIter Iter = m_pAgpmGamble->m_TemplateMap.begin(); Iter != m_pAgpmGamble->m_TemplateMap.end(); Iter++)
		{
		AgpdGamble *pAgpdGamble = &Iter->second;

		AgpmFactors* ppmFactor = ( AgpmFactors* )GetModule( "AgpmFactors" );
		if( !ppmFactor ) return FALSE;

		if (ppmFactor->CheckRace((AuRaceType)ppmFactor->GetRace(&pAgpdCharacter->m_csFactor), &pAgpdGamble->m_pItemTemplate->m_csFactor)
			&& ppmFactor->CheckClass((AuCharClassType)ppmFactor->GetClass(&pAgpdCharacter->m_csFactor), &pAgpdGamble->m_pItemTemplate->m_csFactor))
			{
			//########################
			//if (m_pAgpmGamble->GetGambleItemList(pAgpdGamble, pAgpdCharacter, NULL) > 0)		// at least 1 result item required
				{
				// create and grid item
				AgpdGridItem *pAgpdGridItem = m_pAgpmGrid->CreateGridItem();
				pAgpdGridItem->m_eType = AGPDGRID_ITEM_TYPE_ITEM;
				pAgpdGridItem->m_lItemTID = pAgpdGamble->m_lTID;
				
				m_pAgcmItem->SetGridItemAttachedTexture(pAgpdGamble->m_pItemTemplate);
				
				RwTexture **ppRwTexture = (RwTexture **)(m_pAgpmGrid->GetAttachedModuleData( 
											m_pAgcmUIControl->m_lItemGridTextureADDataIndex, pAgpdGridItem ));

				AgcdItemTemplate *pAgcdItemTemplate = m_pAgcmItem->GetTemplateData(pAgpdGamble->m_pItemTemplate);
				(RwTexture *) *ppRwTexture = pAgcdItemTemplate->m_pTexture;

				m_pAgpmGrid->AddItem(&m_AgpdGrid_TemplateList, pAgpdGridItem);
				m_pAgpmGrid->Add(&m_AgpdGrid_TemplateList, pAgpdGridItem, 1, 1);
				}
			}
		}
	
	// set associated user data count
	m_pAgcdUIUserData_Grid->m_stUserData.m_lCount = m_pAgpmGrid->GetItemCount(&m_AgpdGrid_TemplateList);
	
	return TRUE;
	}


void AgcmUIGamble::RemoveGambleGridItem()
	{
	// for all grid item
	AgpdGridItem *pAgpdGridItem = NULL;
	INT32 lIndex = 0;
	for (pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&m_AgpdGrid_TemplateList, &lIndex);
		 pAgpdGridItem;
		 pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&m_AgpdGrid_TemplateList, &lIndex)
		 )
		{
		// remove grid item
		m_pAgpmGrid->RemoveItem(&m_AgpdGrid_TemplateList, pAgpdGridItem);
		m_pAgpmGrid->DeleteGridItem(pAgpdGridItem);
		}
	
	m_pAgpmGrid->Reset(&m_AgpdGrid_TemplateList);
	}


#define COLOR_WHITE		0xFFFFFFFF
#define COLOR_GRAY		0XFFAEA59C
#define COLOR_RED		0xFFFF0000

void AgcmUIGamble::OpenTooltip(INT32 lTID, INT32 x, INT32 y)
	{
	AgpdGamble *pAgpdGamble = m_pAgpmGamble->GetTemplate(lTID);
	AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
	if (!pAgpdGamble || !pAgpdCharacter)
		return;
		
	m_Tooltip.MoveWindow(x, y, m_Tooltip.w, m_Tooltip.h);

	// set tooltip text
	CHAR	szBuffer[128];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	// name
	sprintf(szBuffer, "%s", pAgpdGamble->m_szName);
	m_Tooltip.AddString(szBuffer, 14, COLOR_WHITE);
	m_Tooltip.AddNewLine(14);

	// race
	
	// class
	
	// cost
	INT32 lCost = m_pAgpmGamble->Cost(pAgpdCharacter, pAgpdGamble);
	INT32 lCharisma = m_pAgpmCharacter->GetCharismaPoint(pAgpdCharacter);
	sprintf(szBuffer, "%d %s", lCost, ClientStr().GetStr(STI_FACTOR_POINT_CHA));
	if (lCharisma >= lCost)
		m_Tooltip.AddString(szBuffer, 14, COLOR_WHITE);
	else
		m_Tooltip.AddString(szBuffer, 14, COLOR_RED);
	m_Tooltip.AddNewLine(14);

	m_Tooltip.AddNewLine(14);
	
	m_Tooltip.ShowWindow(TRUE);
	}

#undef COLOR_WHITE
#undef COLOR_GRAY
#undef COLOR_RED


void AgcmUIGamble::CloseTooltip()
	{
	m_Tooltip.ShowWindow(FALSE);
	m_Tooltip.DeleteAllStringInfo();
	}


CHAR* AgcmUIGamble::GetMessageTxt(eAGCMUIGAMBLE_MESSAGE eMessage)
	{
	if (0 > eMessage || eMessage > AGCMUIGAMBLE_MESSAGE_MAX)
		return NULL;

	return m_pAgcmUIManager2->GetUIMessage(s_szMessage[eMessage]);
	}


void AgcmUIGamble::AddSystemMessage(CHAR *pszMessage)
	{
	if (!pszMessage)
		return;

	AgpdChatData	stChatData;
	ZeroMemory(&stChatData, sizeof(stChatData));

	stChatData.eChatType = AGPDCHATTING_TYPE_SYSTEM_LEVEL1;
	stChatData.szMessage = pszMessage;

	stChatData.lMessageLength = _tcslen(stChatData.szMessage);
		
	m_pAgcmChatting2->AddChatMessage(AGCMCHATTING_TYPE_SYSTEM, &stChatData);
	m_pAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
	}
