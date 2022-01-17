#include "AgcmUIEventItemConvert.h"

#define AGCMUI_ITEMCONVERT_FADE_COLOR				0xffffff
#define AGCMUI_ITEMCONVERT_FADEIN_DURATION			200
#define AGCMUI_ITEMCONVERT_FADEOUT_DURATION			200
#define AGCMUI_ITEMCONVERT_MSG_COLOR				0xffffff
#define AGCMUI_ITEMCONVERT_MSG_INIT_SCALE			10.0f
#define AGCMUI_ITEMCONVERT_MSG_FADEIN_DURATION		500
#define AGCMUI_ITEMCONVERT_MSG_SCALING_DURATION		500
#define AGCMUI_ITEMCONVERT_MSG_FIXED_DURATION		2000
#define AGCMUI_ITEMCONVERT_SUCCESS_MARGIN_BOTTOM	60
#define AGCMUI_ITEMCONVERT_SUCCESS_MARGIN_RIGHT		194
#define AGCMUI_ITEMCONVERT_FAILED_MARGIN_BOTTOM		60
#define AGCMUI_ITEMCONVERT_FAILED_MARGIN_RIGHT		240


AgcmUIEventItemConvert::AgcmUIEventItemConvert()
{
	SetModuleName("AgcmUIEventItemConvert");
	EnableIdle(TRUE);

	m_pcsUserDataGrid				= NULL;
	m_pcsUserDataGridSocket			= NULL;
	m_pcsUserDataConvertCost		= NULL;
	m_pcsUserDataConvertType		= NULL;

	m_lEventOpenConvertMainUI		= 0;
	m_lEventCloseConvertUI			= 0;
	m_lEventOpenConvertUI			= 0;

	m_llConvertCost					= 0;

	m_eConvertType					= AGCMUI_ITEMCONVERT_TYPE_NONE;
	m_lConvertResult				= (-1);

	m_pcsConvertItem				= NULL;

	ZeroMemory(m_szRuneName, sizeof(CHAR) * 64);

	ZeroMemory(&m_stEventPos, sizeof(AuPOS));

	ZeroMemory(&m_stConvertOpenPos, sizeof(AuPOS));

	m_bIsConvertUIOpen				= FALSE;

	m_ulTimeToThrowEvent			= 0;
	m_lEventToThrow					= 0;

	m_pcsPhysicalEditControl		= NULL;
	m_pcsRuneEditControl			= NULL;
	m_pcsSocketEditControl			= NULL;
	
	m_lPrevNumPhysicalConvert		= 0;

	m_bIsRequestConvert				= FALSE;

	m_ulNextRequestConvertTime		= 0;
}

AgcmUIEventItemConvert::~AgcmUIEventItemConvert()
{
}

BOOL AgcmUIEventItemConvert::OnAddModule()
{
	m_pcsAgpmGrid				= (AgpmGrid *)				GetModule("AgpmGrid");
	m_pcsAgpmCharacter			= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmItem				= (AgpmItem *)				GetModule("AgpmItem");
	m_pcsAgpmItemConvert		= (AgpmItemConvert *)		GetModule("AgpmItemConvert");

	m_pcsAgcmCharacter			= (AgcmCharacter *)			GetModule("AgcmCharacter");
	m_pcsAgcmItemConvert		= (AgcmItemConvert *)		GetModule("AgcmItemConvert");
	m_pcsAgcmEventItemConvert	= (AgcmEventItemConvert *)	GetModule("AgcmEventItemConvert");
	m_pcsAgcmUIManager2			= (AgcmUIManager2 *)		GetModule("AgcmUIManager2");

	m_pcsAgcmSound				= (AgcmSound *)				GetModule("AgcmSound");

	if (!m_pcsAgpmGrid ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmItemConvert ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmItemConvert ||
		!m_pcsAgcmEventItemConvert ||
		!m_pcsAgcmUIManager2)
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackRemove(CBRemoveItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackField(CBRemoveItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackBank(CBRemoveItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackEquip(CBRemoveItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackChangeItemOwner(CBRemoveItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackTradeGrid(CBRemoveItem, this))
		return FALSE;

	if (!m_pcsAgpmItemConvert->SetCallbackResultPhysicalConvert(CBResultPhysicalConvert, this))
		return FALSE;
	if (!m_pcsAgpmItemConvert->SetCallbackResultSocketConvert(CBResultSocketConvert, this))
		return FALSE;
	if (!m_pcsAgpmItemConvert->SetCallbackResultSpiritStoneConvert(CBResultSpiritStoneConvert, this))
		return FALSE;
	if (!m_pcsAgpmItemConvert->SetCallbackResultRuneConvert(CBResultRuneConvert, this))
		return FALSE;

	if (!m_pcsAgcmEventItemConvert->SetCallbackEventGrant(CBReceiveGrant, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	if (!AddEvent())
		return FALSE;
	if (!AddFunction())
		return FALSE;
	if (!AddDisplay())
		return FALSE;
	if (!AddUserData())
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventItemConvert::OnInit()
{
	m_csToolTip.m_Property.bTopmost = TRUE;
	m_pcsAgcmUIManager2->AddWindow((AgcWindow *) (&m_csToolTip));
	m_csToolTip.ShowWindow(FALSE);

	m_pcsAgpmGrid->Init(&m_stGrid, 1, 1, 1);
	m_pcsAgpmGrid->Init(&m_stGridSocket, 1, 1, 1);

	m_stGrid.m_lGridType	= AGPDGRID_ITEM_TYPE_ITEM;
	m_stGridSocket.m_lGridType	= AGPDGRID_ITEM_TYPE_ITEM;

	return TRUE;
}

BOOL AgcmUIEventItemConvert::OnDestroy()
{
	m_pcsAgpmGrid->Remove(&m_stGrid);
	m_pcsAgpmGrid->Remove(&m_stGridSocket);

	return TRUE;
}

BOOL AgcmUIEventItemConvert::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmUIEventItemConvert::OnIdle");

	if (m_ulTimeToThrowEvent && m_ulTimeToThrowEvent <= ulClockCount)
	{
		m_ulTimeToThrowEvent	= 0;

		m_pcsAgcmUIManager2->ThrowEvent(m_lEventToThrow);
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::AddEvent()
{
	m_lEventOpenConvertMainUI		= m_pcsAgcmUIManager2->AddEvent("Convert_MainUI_Open");
	if (m_lEventOpenConvertMainUI < 0)
		return FALSE;

	m_lEventCloseConvertUI			= m_pcsAgcmUIManager2->AddEvent("Convert_UI_Close");
	if (m_lEventCloseConvertUI < 0)
		return FALSE;

	m_lEventOpenConvertUI			= m_pcsAgcmUIManager2->AddEvent("Convert_UI_Open");
	if (m_lEventOpenConvertUI < 0)
		return FALSE;

	m_lEventConvertConfirm			= m_pcsAgcmUIManager2->AddEvent("Convert_Confirm");
	if (m_lEventConvertConfirm < 0)
		return FALSE;

	m_lEventSpiritStoneConvertSuccess			= m_pcsAgcmUIManager2->AddEvent("Convert_SS_Result_Success");
	if (m_lEventSpiritStoneConvertSuccess < 0) return FALSE;
	m_lEventSpiritStoneConvertFail				= m_pcsAgcmUIManager2->AddEvent("Convert_SS_Result_Fail");
	if (m_lEventSpiritStoneConvertFail < 0) return FALSE;
	m_lEventSpiritStoneConvertImproperItem		= m_pcsAgcmUIManager2->AddEvent("Convert_SS_Result_ImproperItem");
	if (m_lEventSpiritStoneConvertImproperItem < 0) return FALSE;
	m_lEventSpiritStoneConvertAlreadyFull		= m_pcsAgcmUIManager2->AddEvent("Convert_SS_Result_AlreadyFull");
	if (m_lEventSpiritStoneConvertAlreadyFull < 0) return FALSE;

	m_lEventPhysicalConvertAlreadyFull			= m_pcsAgcmUIManager2->AddEvent("Convert_Physical_Result_AlreadyFull");
	if (m_lEventPhysicalConvertAlreadyFull < 0) return FALSE;
	m_lEventPhysicalConvertImproperItem			= m_pcsAgcmUIManager2->AddEvent("Convert_Physical_Result_ImproperItem");
	if (m_lEventPhysicalConvertImproperItem < 0) return FALSE;

	m_lEventSocketConvertImproperItem			= m_pcsAgcmUIManager2->AddEvent("Convert_Socket_Result_ImproperItem");
	if (m_lEventSocketConvertImproperItem < 0) return FALSE;
	m_lEventSocketConvertNotEnoughMoney			= m_pcsAgcmUIManager2->AddEvent("Convert_Socket_Result_NotEnoughMoney");
	if (m_lEventSocketConvertNotEnoughMoney < 0) return FALSE;
	m_lEventSocketConvertAlreadyFull			= m_pcsAgcmUIManager2->AddEvent("Convert_Socket_Result_AlreadyFull");
	if (m_lEventSocketConvertAlreadyFull < 0) return FALSE;

	m_lEventSocketConvertNotEnoughInvenMoney	= m_pcsAgcmUIManager2->AddEvent("Convert_Socket_NotEnoughInvenMoney");
	if (m_lEventSocketConvertNotEnoughInvenMoney < 0) return FALSE;
	m_lEventSocketConvertEnoughInvenMoney		= m_pcsAgcmUIManager2->AddEvent("Convert_Socket_EnoughInvenMoney");
	if (m_lEventSocketConvertEnoughInvenMoney < 0) return FALSE;

	m_lEventRuneConvertNotEnoughSocket			= m_pcsAgcmUIManager2->AddEvent("Convert_Rune_Result_NotEnoughSocket");
	if (m_lEventRuneConvertNotEnoughSocket < 0) return FALSE;

	m_lEventErrAlreadyFull			= m_pcsAgcmUIManager2->AddEvent("Convert_Err_AlreadyFull");
	if (m_lEventErrAlreadyFull < 0) return FALSE;
	m_lEventErrNotExistItem			= m_pcsAgcmUIManager2->AddEvent("Convert_Err_NotExistItem");
	if (m_lEventErrNotExistItem < 0) return FALSE;
	m_lEventErrInvalidCatalyst		= m_pcsAgcmUIManager2->AddEvent("Convert_Err_InvalidCatalyst");
	if (m_lEventErrInvalidCatalyst < 0) return FALSE;
	m_lEventErrInvalidRune			= m_pcsAgcmUIManager2->AddEvent("Convert_Err_InvalidRune");
	if (m_lEventErrInvalidRune < 0) return FALSE;
	m_lEventErrNotEnoughSocket		= m_pcsAgcmUIManager2->AddEvent("Convert_Err_NotEnoughSocket");
	if (m_lEventErrNotEnoughSocket < 0) return FALSE;
	m_lEventErrLowCharLevel			= m_pcsAgcmUIManager2->AddEvent("Convert_Err_LowCharLevel");
	if (m_lEventErrLowCharLevel < 0) return FALSE;
	m_lEventErrLowItemLevel			= m_pcsAgcmUIManager2->AddEvent("Convert_Err_LowItemLevel");
	if (m_lEventErrLowItemLevel < 0) return FALSE;
	m_lEventErrImproperPart			= m_pcsAgcmUIManager2->AddEvent("Convert_Err_ImproperPart");
	if (m_lEventErrImproperPart < 0) return FALSE;
	m_lEventErrNotEnoughMoney		= m_pcsAgcmUIManager2->AddEvent("Convert_Err_NotEnoughMoney");
	if (m_lEventErrNotEnoughMoney < 0) return FALSE;
	m_lEventErrTryEquipItem			= m_pcsAgcmUIManager2->AddEvent("Convert_Err_TryEquipItem");
	if (m_lEventErrTryEquipItem < 0) return FALSE;
	m_lEventErrAlreadyAntiConvert	= m_pcsAgcmUIManager2->AddEvent("Convert_Err_AlreadyAntiConvert");
	if (m_lEventErrAlreadyAntiConvert < 0) return FALSE;

	m_lEventUpdateNPCConv			= m_pcsAgcmUIManager2->AddEvent("Convert_Update_NPCConv");
	if (m_lEventUpdateNPCConv < 0) return FALSE;
	m_lEventUpdateConvertInfo		= m_pcsAgcmUIManager2->AddEvent("Convert_Update_ConvertInfo");
	if (m_lEventUpdateConvertInfo < 0) return FALSE;
	m_lEventUpdateConvertConfirm	= m_pcsAgcmUIManager2->AddEvent("Convert_Update_ConvertConfirm");
	if (m_lEventUpdateConvertConfirm < 0) return FALSE;

	return TRUE;
}

BOOL AgcmUIEventItemConvert::AddFunction()
{
//	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_OpenTooltip_Physical", CBOpenTooltipPhysical, 0))
//		return FALSE;
//	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_OpenTooltip_Socket", CBOpenTooltipSocket, 0))
//		return FALSE;
//	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_OpenTooltip_Rune", CBOpenTooltipRune, 0))
//		return FALSE;
//	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_CloseTooltip", CBCloseTooltip, 0))
//		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_SetPhysicalConvert", CBSetPhysicalConvert, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_SetSocketConvert", CBSetSocketConvert, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_SetRuneConvert", CBSetRuneConvert, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_AddItem", CBAddConvertGridItem, 1, "Self Grid Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_AddItemSocket", CBAddConvertGridItemSocket, 1, "Self Grid Control"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_Start", CBStartConvert, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_OK", CBConvertOK, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_Cancel", CBConvertCancel, 0))
		return FALSE;

//	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_Update_NPCConv", CBUpdateNPCConv, 1, "NPCConv Edit Control"))
//		return FALSE;
//	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_Update_ConvertInfo", CBUpdateConvertInfo, 1, "ConvertInfo Edit Control"))
//		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_Update_ConvertConfirm", CBUpdateConvertConfirm, 1, "ConvertConfirm Edit Control"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_Set_NPCConv", CBSetNPCConv, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_GetPhysicalEditControl", CBGetPhysicalEditControl, 1, "PhysicalEditControl"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_GetRuneEditControl", CBGetRuneEditControl, 1, "RuneEditControl"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Convert_GetSocketEditControl", CBGetSocketEditControl, 1, "SocketEditControl"))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventItemConvert::AddDisplay()
{
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Display_ConvertItemName", AGCMUI_ITEMCONVERT_DISPLAY_ID_CONVERT_ITEM_NAME, CBDisplayConvertItemName, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Display_ConvertCost", AGCMUI_ITEMCONVERT_DISPLAY_ID_COST, CBDisplayConvertCost, AGCDUI_USERDATA_TYPE_UINT64))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Display_ConvertCostPayResult", AGCMUI_ITEMCONVERT_DISPLAY_ID_COST_PAY_RESULT, CBDisplayConvertCostPayResult, AGCDUI_USERDATA_TYPE_UINT64))
		return FALSE;

//	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Display_ConvertTitle", AGCMUI_ITEMCONVERT_DISPLAY_ID_CONVERT_TITLE, CBDisplayConvertTitle, AGCDUI_USERDATA_TYPE_INT32))
//		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventItemConvert::AddUserData()
{
	m_pcsUserDataGrid			= m_pcsAgcmUIManager2->AddUserData("Convert_Grid",
															   &m_stGrid,
															   sizeof(AgpdGrid),
															   1,
															   AGCDUI_USERDATA_TYPE_GRID);
	if (!m_pcsUserDataGrid)
		return FALSE;

	m_pcsUserDataGridSocket		= m_pcsAgcmUIManager2->AddUserData("Convert_Grid_Socket",
															   &m_stGridSocket,
															   sizeof(AgpdGrid),
															   1,
															   AGCDUI_USERDATA_TYPE_GRID);
	if (!m_pcsUserDataGridSocket)
		return FALSE;

	m_pcsUserDataConvertCost	= m_pcsAgcmUIManager2->AddUserData("Convert_Cost",
																&m_llConvertCost,
																sizeof(INT64),
																1,
																AGCDUI_USERDATA_TYPE_UINT64);
	if (!m_pcsUserDataConvertCost)
		return FALSE;

	m_pcsUserDataConvertType	= m_pcsAgcmUIManager2->AddUserData("Convert_Type",
																&m_eConvertType,
																sizeof(INT32),
																1,
																AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataConvertType)
		return FALSE;

	m_pcsUserDataConvertItem	= m_pcsAgcmUIManager2->AddUserData("Convert_Item",
																m_pcsConvertItem,
																sizeof(AgpdItem *),
																1,
																AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pcsUserDataConvertItem)
		return FALSE;

	m_pcsUserDataConvertResult	= m_pcsAgcmUIManager2->AddUserData("Convert_Result",
																&m_lDummyData,
																sizeof(INT32),
																1,
																AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataConvertResult)
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBReceiveGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)					pData;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)				pCustData;

	if (pcsCharacter != pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		return FALSE;

	pThis->m_eConvertType	= AGCMUI_ITEMCONVERT_TYPE_NONE;
	pThis->m_lConvertResult = (-1);

	pThis->m_pcsConvertItem	= NULL;

	pThis->m_pcsAgpmGrid->Reset(&pThis->m_stGrid);

	pThis->m_pcsApmEventManager->GetBasePos(pcsEvent, &pThis->m_stEventPos);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenConvertMainUI);

//	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpdateNPCConv);

	pThis->m_stConvertOpenPos	= pcsCharacter->m_stPos;

	pThis->m_bIsConvertUIOpen	= TRUE;

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmUIEventItemConvert::CBSelfUpdatePosition");

	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)				pData;

	if (!pThis->m_bIsConvertUIOpen)
		return TRUE;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos, pThis->m_stConvertOpenPos);

	if ((INT32) fDistance < AGCMUIEVENTITEMCONVERT_CLOSE_UI_DISTANCE)
		return TRUE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseConvertUI);

	pThis->m_bIsConvertUIOpen	= FALSE;

	return TRUE;
}

//BOOL AgcmUIEventItemConvert::CBOpenTooltipPhysical(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	if (!pClass || !pcsSourceControl)
//		return FALSE;
//
//	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;
//
//	pThis->m_csToolTip.MoveWindow(pThis->m_pcsAgcmUIManager2->m_v2dCurMousePos.x,
//								  pThis->m_pcsAgcmUIManager2->m_v2dCurMousePos.y, 
//								  pThis->m_csToolTip.w,
//								  pThis->m_csToolTip.h);
//
//	DWORD	dwColor	= 0xffffffff;
//
//	CHAR	*szMessage	= pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICALTOOLTIP);
//	if (szMessage)
//	{
//		INT32	lLineNumber = 1;
//		CHAR	szBuffer[128];
//		ZeroMemory(szBuffer, sizeof(CHAR) * 128);
//
//		while (pThis->m_pcsAgcmUIManager2->GetSeparateLine(szMessage, lLineNumber, szBuffer, 128))
//		{
//			pThis->m_csToolTip.AddString( szBuffer, 14, dwColor )						;
//			pThis->m_csToolTip.AddNewLine( 14 );
//
//			ZeroMemory(szBuffer, sizeof(CHAR) * 128);
//
//			++lLineNumber;
//		}
//	}
//	
//	pThis->m_csToolTip.ShowWindow(TRUE);
//
//	return TRUE;
//}
//
//BOOL AgcmUIEventItemConvert::CBOpenTooltipSocket(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	if (!pClass || !pcsSourceControl)
//		return FALSE;
//
//	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;
//
//	pThis->m_csToolTip.MoveWindow(pThis->m_pcsAgcmUIManager2->m_v2dCurMousePos.x,
//								  pThis->m_pcsAgcmUIManager2->m_v2dCurMousePos.y, 
//								  pThis->m_csToolTip.w,
//								  pThis->m_csToolTip.h);
//
//	DWORD	dwColor	= 0xffffffff;
//
//	CHAR	*szMessage	= pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_SOCKETTOOLTIP);
//	if (szMessage)
//	{
//		INT32	lLineNumber = 1;
//		CHAR	szBuffer[128];
//		ZeroMemory(szBuffer, sizeof(CHAR) * 128);
//
//		while (pThis->m_pcsAgcmUIManager2->GetSeparateLine(szMessage, lLineNumber, szBuffer, 128))
//		{
//			pThis->m_csToolTip.AddString( szBuffer, 14, dwColor )						;
//			pThis->m_csToolTip.AddNewLine( 14 );
//
//			ZeroMemory(szBuffer, sizeof(CHAR) * 128);
//
//			++lLineNumber;
//		}
//	}
//	
//	pThis->m_csToolTip.ShowWindow(TRUE);
//
//	return TRUE;
//}
//
//BOOL AgcmUIEventItemConvert::CBOpenTooltipRune(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	if (!pClass || !pcsSourceControl)
//		return FALSE;
//
//	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;
//
//	pThis->m_csToolTip.MoveWindow(pThis->m_pcsAgcmUIManager2->m_v2dCurMousePos.x,
//								  pThis->m_pcsAgcmUIManager2->m_v2dCurMousePos.y, 
//								  pThis->m_csToolTip.w,
//								  pThis->m_csToolTip.h);
//
//	DWORD	dwColor	= 0xffffffff;
//
//	CHAR	*szMessage	= pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_RUNETOOLTIP);
//	if (szMessage)
//	{
//		INT32	lLineNumber = 1;
//		CHAR	szBuffer[128];
//		ZeroMemory(szBuffer, sizeof(CHAR) * 128);
//
//		while (pThis->m_pcsAgcmUIManager2->GetSeparateLine(szMessage, lLineNumber, szBuffer, 128))
//		{
//			pThis->m_csToolTip.AddString( szBuffer, 14, dwColor )						;
//			pThis->m_csToolTip.AddNewLine( 14 );
//
//			ZeroMemory(szBuffer, sizeof(CHAR) * 128);
//
//			++lLineNumber;
//		}
//	}
//	
//	pThis->m_csToolTip.ShowWindow(TRUE);
//
//	return TRUE;
//}
//
//BOOL AgcmUIEventItemConvert::CBCloseTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	if (!pClass || !pcsSourceControl)
//		return FALSE;
//
//	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;
//
//	pThis->m_csToolTip.ShowWindow(FALSE);
//	pThis->m_csToolTip.DeleteAllStringInfo();
//
//	return TRUE;
//}

BOOL AgcmUIEventItemConvert::CBAddConvertGridItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;
	AgcdUIControl			*pcsControl			= (AgcdUIControl *)				pData1;

	if (!pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID) ||
		!pcsControl->m_pcsBase)
		return FALSE;

	AcUIGrid				*pcsUIGrid			= (AcUIGrid *)					pcsControl->m_pcsBase;

	AgpdGridSelectInfo		*pcsDragInfo		= pcsUIGrid->GetDragDropMessageInfo();
	if (!pcsDragInfo || !pcsDragInfo->pGridItem ||
		pcsDragInfo->pGridItem->m_eType != AGPDGRID_ITEM_TYPE_ITEM)
		return FALSE;

	AgpdItem				*pcsDragItem		= pThis->m_pcsAgpmItem->GetItem(pcsDragInfo->pGridItem->m_lItemID);
	if (!pcsDragItem)
		return FALSE;

	if (pcsDragItem->m_eStatus != AGPDITEM_STATUS_INVENTORY)
	{
		if (pcsDragItem->m_eStatus == AGPDITEM_STATUS_EQUIP)
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventErrTryEquipItem);

		return FALSE;
	}

	BOOL	bIsAddItem	= FALSE;

//	switch (pcsDragInfo->lGridColumn) {
//	case 0:
		if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_SOCKET)
		{
			AgpdItemConvertSocketResult	eResult	= pThis->m_pcsAgpmItemConvert->IsSocketConvertable(pcsDragItem);

			if (eResult == AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS ||
				eResult == AGPDITEMCONVERT_SOCKET_RESULT_NOT_ENOUGH_MONEY)
				bIsAddItem	= TRUE;
			else
				pThis->ThrowEventSocketConvertable(eResult);

			pThis->RecalcConvertCost(pcsDragItem);
		}
		else if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_RUNE)
		{
			AgpdItemConvertRuneResult	eResult	= pThis->m_pcsAgpmItemConvert->IsRuneConvertable(pcsDragItem);

			if (eResult == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
				bIsAddItem	= TRUE;
			else
				pThis->ThrowEventRuneConvertable(eResult);
		}
		else if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_PHYSICAL)
		{
			AgpdItemConvertResult		eResult	= pThis->m_pcsAgpmItemConvert->IsPhysicalConvertable(pcsDragItem);

			if (eResult == AGPDITEMCONVERT_RESULT_SUCCESS)
				bIsAddItem	= TRUE;
			else
				pThis->ThrowEventPhysicalConvertable(eResult);
		}

		if (bIsAddItem)
			pThis->m_pcsConvertItem	= pcsDragItem;
//		break;
//
//	case 1:
//		if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_RUNE)
//		{
//			if (pThis->m_pcsAgpmItemConvert->IsValidRuneItem(pcsDragItem) == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
//				bIsAddItem	= TRUE;
//		}
//		else if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_PHYSICAL)
//		{
//			if (pThis->m_pcsAgpmItemConvert->IsValidCatalyst(pcsDragItem) == AGPDITEMCONVERT_RESULT_SUCCESS)
//				bIsAddItem	= TRUE;
//		}
//
//		break;
//
//	case 2:
//		break;
//	}

	if (bIsAddItem)
	{
		pThis->m_pcsAgpmGrid->DeleteItem(&pThis->m_stGrid, NULL);
		pThis->m_pcsAgpmGrid->Clear(&pThis->m_stGrid, 0, 0, 0, 1, 1);

		pThis->m_pcsAgpmGrid->AddItem(&pThis->m_stGrid, pcsDragItem->m_pcsGridItem);
		pThis->m_pcsAgpmGrid->Add(&pThis->m_stGrid, 0, 0, 0, pcsDragItem->m_pcsGridItem, 1, 1);

		pThis->RefreshUserDataGrid();
		pThis->RefreshUserDataConvertItem();

		if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_RUNE)
			pThis->SetRuneResult(AGPDITEMCONVERT_RUNE_RESULT_NONE);
		else if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_SOCKET)
			pThis->SetSocketResult(AGPDITEMCONVERT_SOCKET_RESULT_NONE);
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBAddConvertGridItemSocket(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;
	AgcdUIControl			*pcsControl			= (AgcdUIControl *)				pData1;

	if (!pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID) ||
		!pcsControl->m_pcsBase)
		return FALSE;

	AcUIGrid				*pcsUIGrid			= (AcUIGrid *)					pcsControl->m_pcsBase;

	AgpdGridSelectInfo		*pcsDragInfo		= pcsUIGrid->GetDragDropMessageInfo();
	if (!pcsDragInfo || !pcsDragInfo->pGridItem ||
		pcsDragInfo->pGridItem->m_eType != AGPDGRID_ITEM_TYPE_ITEM)
		return FALSE;

	AgpdItem				*pcsDragItem		= pThis->m_pcsAgpmItem->GetItem(pcsDragInfo->pGridItem->m_lItemID);
	if (!pcsDragItem)
		return FALSE;

	if (pcsDragItem->m_eStatus != AGPDITEM_STATUS_INVENTORY)
	{
		if (pcsDragItem->m_eStatus == AGPDITEM_STATUS_EQUIP)
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventErrTryEquipItem);

		return FALSE;
	}

	BOOL	bIsAddItem	= FALSE;

//	switch (pcsDragInfo->lGridColumn) {
//	case 0:
//		if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_SOCKET)
//		{
//			AgpdItemConvertSocketResult	eResult	= pThis->m_pcsAgpmItemConvert->IsSocketConvertable(pcsDragItem);
//
//			if (eResult == AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS ||
//				eResult == AGPDITEMCONVERT_SOCKET_RESULT_NOT_ENOUGH_MONEY)
//				bIsAddItem	= TRUE;
//			else
//				pThis->ThrowEventSocketConvertable(eResult);
//
//			pThis->RecalcConvertCost(pcsDragItem);
//		}
//		else if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_RUNE)
//		{
//			AgpdItemConvertRuneResult	eResult	= pThis->m_pcsAgpmItemConvert->IsRuneConvertable(pcsDragItem);
//
//			if (eResult == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
//				bIsAddItem	= TRUE;
//			else
//				pThis->ThrowEventRuneConvertable(eResult);
//		}
//		else if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_PHYSICAL)
//		{
//			AgpdItemConvertResult		eResult	= pThis->m_pcsAgpmItemConvert->IsPhysicalConvertable(pcsDragItem);
//
//			if (eResult == AGPDITEMCONVERT_RESULT_SUCCESS)
//				bIsAddItem	= TRUE;
//			else
//				pThis->ThrowEventPhysicalConvertable(eResult);
//		}
//
//		pThis->m_pcsConvertItem	= pcsDragItem;
//		break;
//
//	case 1:
		if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_RUNE)
		{
			if (pThis->m_pcsAgpmItemConvert->IsValidRuneItem(pcsDragItem) == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
				bIsAddItem	= TRUE;
		}
		else if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_PHYSICAL)
		{
			if (pThis->m_pcsAgpmItemConvert->IsValidCatalyst(pcsDragItem) == AGPDITEMCONVERT_RESULT_SUCCESS)
				bIsAddItem	= TRUE;
		}
//
//		break;
//
//	case 2:
//		break;
//	}

	if (bIsAddItem)
	{
		pThis->m_pcsAgpmGrid->DeleteItem(&pThis->m_stGridSocket, NULL);
		pThis->m_pcsAgpmGrid->Clear(&pThis->m_stGridSocket, 0, 0, 0, 1, 1);

		pThis->m_pcsAgpmGrid->AddItem(&pThis->m_stGridSocket, pcsDragItem->m_pcsGridItem);
		pThis->m_pcsAgpmGrid->Add(&pThis->m_stGridSocket, 0, 0, 0, pcsDragItem->m_pcsGridItem, 1, 1);

		pThis->RefreshUserDataGridSocket();
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBStartConvert(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;

	switch (pThis->m_eConvertType) {
	case AGCMUI_ITEMCONVERT_TYPE_PHYSICAL:
		{
			if (pThis->m_ulNextRequestConvertTime > pThis->GetClockCount())
				return TRUE;

			return pThis->CheckPhysicalConvert();
		}
		break;

	case AGCMUI_ITEMCONVERT_TYPE_SOCKET:
		{
			return pThis->CheckSocketConvert();
		}
		break;

	case AGCMUI_ITEMCONVERT_TYPE_RUNE:
		{
			return pThis->CheckRuneConvert();
		}
		break;
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBConvertOK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;

	switch (pThis->m_eConvertType) {
	case AGCMUI_ITEMCONVERT_TYPE_PHYSICAL:
		{
			if (pThis->m_bIsRequestConvert)
				return TRUE;

			if (pThis->m_ulNextRequestConvertTime > pThis->GetClockCount())
				return TRUE;

			BOOL	bResult	= pThis->StartPhysicalConvert();

			if (bResult)
			{
				pThis->m_bIsRequestConvert	= TRUE;
				pThis->m_ulNextRequestConvertTime	= pThis->GetClockCount() + 3000;
			}

			return bResult;
		}
		break;

	case AGCMUI_ITEMCONVERT_TYPE_SOCKET:
		{
			return pThis->StartSocketConvert();
		}
		break;

	case AGCMUI_ITEMCONVERT_TYPE_RUNE:
		{
			return pThis->StartRuneConvert();
		}
		break;
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBConvertCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	return TRUE;
}

//BOOL AgcmUIEventItemConvert::CBUpdateNPCConv(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	if (!pClass || !pData1)
//		return FALSE;
//
//	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;
//	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)				pData1;
//
//	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
//
//	// 현재 위치로 동네 이름을 가져오는 코드가 없다. 구래서.. 어쩔 수 없이 일단 하드코딩 한다.
//
//	CHAR	szBuffer[256];
//	ZeroMemory(szBuffer, sizeof(CHAR) * 256);
//
//	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
//
//	sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_NPC_CONVERSATION));
//
//	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
//
//	return TRUE;
//}

//BOOL AgcmUIEventItemConvert::CBUpdateConvertInfo(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	if (!pClass || !pData1)
//		return FALSE;
//
//	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;
//	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)				pData1;
//
//	switch (pThis->m_eConvertType) {
//	case AGCMUI_ITEMCONVERT_TYPE_PHYSICAL:
//		{
//			return pThis->UpdatePhysicalConvertInfo(pcsEditControl);
//		}
//		break;
//
//	case AGCMUI_ITEMCONVERT_TYPE_SOCKET:
//		{
//			return pThis->UpdateSocketConvertInfo(pcsEditControl);
//		}
//		break;
//
//	case AGCMUI_ITEMCONVERT_TYPE_RUNE:
//		{
//			return pThis->UpdateRuneConvertInfo(pcsEditControl);
//		}
//		break;
//	}
//
//	return TRUE;
//}

BOOL AgcmUIEventItemConvert::CBUpdateConvertConfirm(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)				pData1;

	if (!pThis->m_pcsConvertItem)
		return FALSE;

	switch (pThis->m_eConvertType) {
	case AGCMUI_ITEMCONVERT_TYPE_PHYSICAL:
		{
			return pThis->UpdatePhysicalConvertConfirm(pcsEditControl, pThis->m_pcsConvertItem);
		}
		break;

	case AGCMUI_ITEMCONVERT_TYPE_SOCKET:
		{
			return pThis->UpdateSocketConvertConfirm(pcsEditControl, pThis->m_pcsConvertItem);
		}
		break;

	case AGCMUI_ITEMCONVERT_TYPE_RUNE:
		{
			return pThis->UpdateRuneConvertConfirm(pcsEditControl, pThis->m_pcsConvertItem);
		}
		break;
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBSetNPCConv(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpdateNPCConv);

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventItemConvert	*pThis					= (AgcmUIEventItemConvert *)	pClass;
	AgpdItem				*pcsItem				= (AgpdItem *)					pData;

	if (pThis->m_eModuleStatus == APMODULE_STATUS_DESTROYED)
		return TRUE;

	if (pcsItem->m_pcsCharacter != pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		return TRUE;

	// m_stGrid에 들어있는 넘인지 본다.

	pThis->m_pcsAgpmGrid->RemoveItem(&pThis->m_stGrid, pcsItem->m_pcsGridItem);
	pThis->m_pcsAgpmGrid->RemoveItem(&pThis->m_stGridSocket, pcsItem->m_pcsGridItem);

	pThis->RefreshUserDataGrid();
	pThis->RefreshUserDataGridSocket();

	if (pThis->m_eConvertType == AGCMUI_ITEMCONVERT_TYPE_SOCKET)
		pThis->RecalcConvertCost(NULL);

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBDisplayConvertItemName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass ||
		eType != AGCDUI_USERDATA_TYPE_STRING ||
		lID != AGCMUI_ITEMCONVERT_DISPLAY_ID_CONVERT_ITEM_NAME)
		return FALSE;

	AgcmUIEventItemConvert	*pThis		= (AgcmUIEventItemConvert *)	pClass;

	if (pThis->m_pcsConvertItem && pThis->m_pcsConvertItem->m_pcsItemTemplate )
	{
		sprintf(szDisplay, "%s", ((AgpdItemTemplate *) pThis->m_pcsConvertItem->m_pcsItemTemplate)->m_szName);

		if (pThis->m_pcsAgpmItemConvert->GetNumPhysicalConvert(pThis->m_pcsConvertItem) > 0)
			sprintf(szDisplay + strlen(szDisplay), " + %d", pThis->m_pcsAgpmItemConvert->GetNumPhysicalConvert(pThis->m_pcsConvertItem));

		// Socket Convert
		if (pThis->m_pcsAgpmItemConvert->GetNumSocket(pThis->m_pcsConvertItem) > 0)
			sprintf(szDisplay + strlen(szDisplay), " [%d/%d]", pThis->m_pcsAgpmItemConvert->GetNumConvertedSocket(pThis->m_pcsConvertItem), pThis->m_pcsAgpmItemConvert->GetNumSocket(pThis->m_pcsConvertItem));
	}
	else
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_INIT_TITLE));

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBDisplayConvertCost(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass ||
		!pData ||
		eType != AGCDUI_USERDATA_TYPE_UINT64 ||
		lID != AGCMUI_ITEMCONVERT_DISPLAY_ID_COST)
		return FALSE;

	INT32	lIndex	= 0;

	INT64	llNumber	= *(INT64 *) pData;

//	if (llNumber < 1000)
//	{
		sprintf(szDisplay + strlen(szDisplay), "%d", (INT32) llNumber);
//	}
//	else
//	{
//		while (llNumber >= 1000)
//		{
//			llNumber	/= 1000;
//			++lIndex;
//		}
//
//		llNumber	= *(INT64 *) pData;
//
//		sprintf(szDisplay + strlen(szDisplay), "%d,", (INT32) (llNumber / pow(1000, lIndex)));
//		llNumber	= llNumber % (INT64) (pow(1000, lIndex));
//
//		if (lIndex > 1)
//		{
//			for (int i = 1; i < lIndex; ++i)
//			{
//				sprintf(szDisplay + strlen(szDisplay), "%3.d,", (INT32) (llNumber / pow(1000, lIndex - i)));
//				llNumber	= llNumber % (INT64) (pow(1000, lIndex - i));
//			}
//		}
//
//		sprintf(szDisplay + strlen(szDisplay), "%3.d", (INT32) llNumber);
//	}

	//sprintf(szDisplay, "%I64d", *(INT64 *) pData);

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBDisplayConvertCostPayResult(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass ||
		!pData ||
		eType != AGCDUI_USERDATA_TYPE_UINT64 ||
		lID != AGCMUI_ITEMCONVERT_DISPLAY_ID_COST_PAY_RESULT)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;

	INT64	llCost		= *(INT64 *) pData;

	INT64	llInvenMoney	= 0;
	pThis->m_pcsAgpmCharacter->GetMoney(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), &llInvenMoney);

	sprintf(szDisplay, "%d", (INT32) (llInvenMoney - llCost));

	return TRUE;
}

//BOOL AgcmUIEventItemConvert::CBDisplayConvertTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
//{
//	if (!pClass ||
//		lID != AGCMUI_ITEMCONVERT_DISPLAY_ID_CONVERT_TITLE)
//		return FALSE;
//
//	AgcmUIEventItemConvert	*	pThis						= (AgcmUIEventItemConvert *)	pClass;
//	CHAR *						szMessage;
//
//	switch (pThis->m_eConvertType) {
//	case AGCMUI_ITEMCONVERT_TYPE_PHYSICAL:
//		szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_TITLE_PHYSICAL);
//		sprintf(szDisplay, szMessage ? szMessage : "");
//		break;
//
//	case AGCMUI_ITEMCONVERT_TYPE_SOCKET:
//		szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_TITLE_SOCKET);
//		sprintf(szDisplay, szMessage ? szMessage : "");
//		break;
//
//	case AGCMUI_ITEMCONVERT_TYPE_RUNE:
//		szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_TITLE_RUNE);
//		sprintf(szDisplay, szMessage ? szMessage : "");
//		break;
//
//	default:
//		szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_TITLE);
//		sprintf(szDisplay, szMessage ? szMessage : "");
//		break;
//	}
//
//	return TRUE;
//}

BOOL AgcmUIEventItemConvert::CBDisplayConvertPhysicalResult(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;

	return TRUE;
}

BOOL AgcmUIEventItemConvert::RefreshUserDataGrid()
{
	m_pcsUserDataGrid->m_stUserData.m_pvData	= &m_stGrid;

	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataGrid);
}

BOOL AgcmUIEventItemConvert::RefreshUserDataGridSocket()
{
	m_pcsUserDataGridSocket->m_stUserData.m_pvData	= &m_stGridSocket;

	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataGridSocket);
}

BOOL AgcmUIEventItemConvert::RefreshUserDataConvertCost()
{
	m_pcsUserDataConvertCost->m_stUserData.m_pvData	= &m_llConvertCost;

	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataConvertCost);
}

BOOL AgcmUIEventItemConvert::RefreshUserDataConvertType()
{
	m_pcsUserDataConvertType->m_stUserData.m_pvData	= &m_eConvertType;

	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataConvertType);
}

BOOL AgcmUIEventItemConvert::RefreshUserDataConvertItem()
{
	m_pcsUserDataConvertItem->m_stUserData.m_pvData	= m_pcsConvertItem;

	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataConvertItem);
}

BOOL AgcmUIEventItemConvert::RefreshUserDataConvertResult()
{
	m_pcsUserDataConvertResult->m_stUserData.m_pvData	= &m_lDummyData;

	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataConvertResult);
}

BOOL AgcmUIEventItemConvert::PlayEffectSuccess(BOOL bThrowEvent)
{
	if (m_pcsAgcmSound)
		m_pcsAgcmSound->PlaySampleSound("SOUND\\UI\\U_ST_SC.WAV");

	m_pcsAgcmUIManager2->m_csEffectWindow.SetEffectFade(AGCMUI_ITEMCONVERT_FADE_COLOR, AGCMUI_ITEMCONVERT_FADEIN_DURATION, AGCMUI_ITEMCONVERT_FADEOUT_DURATION);
	m_pcsAgcmUIManager2->m_csEffectWindow.SetEffectImage("Success", AGCMUI_ITEMCONVERT_MSG_COLOR, AGCMUI_ITEMCONVERT_MSG_INIT_SCALE, AGCMUI_ITEMCONVERT_MSG_FADEIN_DURATION, AGCMUI_ITEMCONVERT_MSG_SCALING_DURATION, AGCMUI_ITEMCONVERT_MSG_FIXED_DURATION, AGCMUI_ITEMCONVERT_SUCCESS_MARGIN_BOTTOM, AGCMUI_ITEMCONVERT_SUCCESS_MARGIN_RIGHT);

	if (bThrowEvent)
	{
		m_lEventToThrow			= m_lEventUpdateConvertInfo;
		m_ulTimeToThrowEvent	= AGCMUI_ITEMCONVERT_MSG_FADEIN_DURATION + AGCMUI_ITEMCONVERT_MSG_SCALING_DURATION + AGCMUI_ITEMCONVERT_MSG_FIXED_DURATION;
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::PlayEffectFailed(BOOL bThrowEvent)
{
	if (m_pcsAgcmSound)
		m_pcsAgcmSound->PlaySampleSound("SOUND\\UI\\U_ST_FL.WAV");

	m_pcsAgcmUIManager2->m_csEffectWindow.SetEffectFade(AGCMUI_ITEMCONVERT_FADE_COLOR, AGCMUI_ITEMCONVERT_FADEIN_DURATION, AGCMUI_ITEMCONVERT_FADEOUT_DURATION);
	m_pcsAgcmUIManager2->m_csEffectWindow.SetEffectImage("Failed", AGCMUI_ITEMCONVERT_MSG_COLOR, AGCMUI_ITEMCONVERT_MSG_INIT_SCALE, AGCMUI_ITEMCONVERT_MSG_FADEIN_DURATION, AGCMUI_ITEMCONVERT_MSG_SCALING_DURATION, AGCMUI_ITEMCONVERT_MSG_FIXED_DURATION, AGCMUI_ITEMCONVERT_FAILED_MARGIN_BOTTOM, AGCMUI_ITEMCONVERT_FAILED_MARGIN_RIGHT);

	if (bThrowEvent)
	{
		m_lEventToThrow			= m_lEventUpdateConvertInfo;
		m_ulTimeToThrowEvent	= AGCMUI_ITEMCONVERT_MSG_FADEIN_DURATION + AGCMUI_ITEMCONVERT_MSG_SCALING_DURATION + AGCMUI_ITEMCONVERT_MSG_FIXED_DURATION;
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBGetPhysicalEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;

	pThis->m_pcsPhysicalEditControl				= (AgcdUIControl *)				pData1;

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBGetRuneEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;

	pThis->m_pcsRuneEditControl					= (AgcdUIControl *)				pData1;

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBGetSocketEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;

	pThis->m_pcsSocketEditControl				= (AgcdUIControl *)				pData1;

	return TRUE;
}