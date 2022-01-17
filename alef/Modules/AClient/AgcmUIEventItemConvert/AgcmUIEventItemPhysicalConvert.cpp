#include "AgcmUIEventItemConvert.h"

BOOL AgcmUIEventItemConvert::CBSetPhysicalConvert(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;

	return pThis->InitPhysicalConvertUI();

//	pThis->RefreshUserDataConvertType();
//
//	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpdateConvertInfo);
//
//	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenConvertUI);
}

BOOL AgcmUIEventItemConvert::InitPhysicalConvertUI()
{
	m_eConvertType	= AGCMUI_ITEMCONVERT_TYPE_PHYSICAL;

	m_pcsAgpmGrid->Reset(&m_stGridSocket);

	// 강화축성제가 인벤토리에 있나 살펴본다. 있음 m_stGrid에 넣는다.

	// 2007.02.27. steeple
	AgpdItem	*pcsItem	= m_pcsAgpmItem->GetExistCatalyst(m_pcsAgcmCharacter->GetSelfCharacter());

	if (pcsItem)
	{
		m_pcsAgpmGrid->AddItem(&m_stGridSocket, pcsItem->m_pcsGridItem);
		m_pcsAgpmGrid->Add(&m_stGridSocket, 0, 0, 0, pcsItem->m_pcsGridItem, 1, 1);
	}

	m_llConvertCost		= 0;

	m_lConvertResult	= (-1);

	RefreshUserDataGrid();
	RefreshUserDataGridSocket();
	RefreshUserDataConvertCost();
	RefreshUserDataConvertItem();

	SetPhysicalResult(AGPDITEMCONVERT_RESULT_NONE);

	// 그 외에 있는 Display 데이타들을 세팅한다.
	//
	//
	//
	//
	//
	//

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CheckPhysicalConvert()
{
	// 강화에 필요한 조건이 만족하는지 살펴본다.

	// 1. 강화 대상이 있는지 본다.
	AgpdGridItem	*pcsConvertGridItem		= m_pcsAgpmGrid->GetItem(&m_stGrid, 0, 0, 0);
	if (!pcsConvertGridItem)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrNotExistItem);
		return FALSE;
	}

	AgpdItem		*pcsConvertItem			= m_pcsAgpmItem->GetItem(pcsConvertGridItem->m_lItemID);
	if (!pcsConvertItem)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrNotExistItem);
		return FALSE;
	}

	// 2. 강화축성제가 있는지 본다.
	AgpdGridItem	*pcsCatalystGridItem	= m_pcsAgpmGrid->GetItem(&m_stGridSocket, 0, 0, 0);
	if (!pcsCatalystGridItem)
	{
		// 2007.02.27. steeple
		AgpdItem	*pcsItem	= m_pcsAgpmItem->GetExistCatalyst(m_pcsAgcmCharacter->GetSelfCharacter());
		if (pcsItem)
		{
			m_pcsAgpmGrid->AddItem(&m_stGridSocket, pcsItem->m_pcsGridItem);
			m_pcsAgpmGrid->Add(&m_stGridSocket, 0, 0, 0, pcsItem->m_pcsGridItem, 1, 1);

			RefreshUserDataGridSocket();

			pcsCatalystGridItem	= pcsItem->m_pcsGridItem;
		}

		if (!pcsCatalystGridItem)
		{
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidCatalyst);
			return FALSE;
		}
	}

	AgpdItem		*pcsCatalystItem		= m_pcsAgpmItem->GetItem(pcsCatalystGridItem->m_lItemID);
	if (!pcsCatalystItem)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidCatalyst);
		return FALSE;
	}

	AgpdItemConvertResult	eResult	= m_pcsAgcmItemConvert->IsPhysicalConvertable(pcsConvertItem, pcsCatalystItem);

	switch (eResult) {
//	case AGPDITEMCONVERT_RESULT_IS_ALREADY_FULL:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventPhysicalConvertAlreadyFull);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RESULT_IS_EGO_ITEM:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventPhysicalConvertImproperItem);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RESULT_INVALID_CATALYST:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidCatalyst);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RESULT_FAILED:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventPhysicalConvertImproperItem);
//		}
//		break;

	case AGPDITEMCONVERT_RESULT_SUCCESS:
		{
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventUpdateConvertConfirm);

			m_pcsAgcmUIManager2->ThrowEvent(m_lEventConvertConfirm);
		}
		break;

	default:
		SetPhysicalResult(eResult);
		break;
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::ThrowEventPhysicalConvertable(AgpdItemConvertResult eResult)
{
//	switch (eResult) {
//	case AGPDITEMCONVERT_RESULT_IS_ALREADY_FULL:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventPhysicalConvertAlreadyFull);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RESULT_IS_EGO_ITEM:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventPhysicalConvertImproperItem);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RESULT_INVALID_CATALYST:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidCatalyst);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RESULT_FAILED:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventPhysicalConvertImproperItem);
//		}
//		break;
//	}

	SetPhysicalResult(eResult);

	return TRUE;
}

BOOL AgcmUIEventItemConvert::StartPhysicalConvert()
{
	// 강화에 필요한 조건이 만족하는지 살펴본다.

	// 1. 강화 대상이 있는지 본다.
	AgpdGridItem	*pcsConvertGridItem		= m_pcsAgpmGrid->GetItem(&m_stGrid, 0, 0, 0);
	if (!pcsConvertGridItem)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrNotExistItem);
		return FALSE;
	}

	AgpdItem		*pcsConvertItem			= m_pcsAgpmItem->GetItem(pcsConvertGridItem->m_lItemID);
	if (!pcsConvertItem)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrNotExistItem);
		return FALSE;
	}

	// 2. 강화축성제가 있는지 본다.
	AgpdGridItem	*pcsCatalystGridItem	= m_pcsAgpmGrid->GetItem(&m_stGridSocket, 0, 0, 0);
	if (!pcsCatalystGridItem)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidCatalyst);
		return FALSE;
	}

	AgpdItem		*pcsCatalystItem		= m_pcsAgpmItem->GetItem(pcsCatalystGridItem->m_lItemID);
	if (!pcsCatalystItem)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidCatalyst);
		return FALSE;
	}

	AgpdItemConvertResult	eResult	= m_pcsAgcmItemConvert->PhysicalConvert(pcsConvertItem, pcsCatalystItem);

	switch (eResult) {
	case AGPDITEMCONVERT_RESULT_IS_ALREADY_FULL:
		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventPhysicalConvertAlreadyFull);
			SetPhysicalResult(eResult);
		}
		break;

	case AGPDITEMCONVERT_RESULT_IS_EGO_ITEM:
		{
		}
		break;

	case AGPDITEMCONVERT_RESULT_INVALID_CATALYST:
		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidCatalyst);
			SetPhysicalResult(eResult);
		}
		break;

	case AGPDITEMCONVERT_RESULT_FAILED:
		{
		}
		break;

	case AGPDITEMCONVERT_RESULT_SUCCESS:
		{
			m_lPrevNumPhysicalConvert	= m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsConvertItem);

			return TRUE;
		}
		break;
	}

	return FALSE;
}

//BOOL AgcmUIEventItemConvert::UpdatePhysicalConvertInfo(AgcdUIControl *pcsEditControl)
//{
//	if (!pcsEditControl)
//		return FALSE;
//
//	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetLineDelimiter(m_pcsAgcmUIManager2->GetLineDelimiter());
//
//	CHAR	szBuffer[256];
//	ZeroMemory(szBuffer, sizeof(CHAR) * 256);
//
//	switch (m_lConvertResult) {
//	case AGPDITEMCONVERT_RESULT_SUCCESS:
//		{
//			CHAR	*szMessage	= m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICAL_SUCCESS);
//			if (!szMessage)
//				return FALSE;
//
//			AgpdGridItem	*pcsGridItem	= m_pcsAgpmGrid->GetItem(&m_stGrid, 0, 0, 0);
//			if (!pcsGridItem)
//				return FALSE;
//
//			AgpdItem		*pcsConvertItem	= m_pcsAgpmItem->GetItem(pcsGridItem->m_lItemID);
//			if (!pcsConvertItem)
//				return FALSE;
//
//			CHAR	szItemInfoOld[32];
//			CHAR	szItemInfoNew[32];
//
//			ZeroMemory(szItemInfoOld, sizeof(CHAR) * 32);
//			ZeroMemory(szItemInfoNew, sizeof(CHAR) * 32);
//
//			if (m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsConvertItem) - 1 > 0)
//			{
//				sprintf(szItemInfoOld, "%s +%d [%d/%d]", ((AgpdItemTemplate *) pcsConvertItem->m_pcsItemTemplate)->m_szName,
//													 m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsConvertItem) - 1,
//													 m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsConvertItem),
//													 m_pcsAgpmItemConvert->GetNumSocket(pcsConvertItem));
//			}
//			else
//			{
//				sprintf(szItemInfoOld, "%s [%d/%d]", ((AgpdItemTemplate *) pcsConvertItem->m_pcsItemTemplate)->m_szName,
//													 m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsConvertItem),
//													 m_pcsAgpmItemConvert->GetNumSocket(pcsConvertItem));
//			}
//
//			if (m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsConvertItem) > 0)
//			{
//				sprintf(szItemInfoNew, "%s +%d [%d/%d]", ((AgpdItemTemplate *) pcsConvertItem->m_pcsItemTemplate)->m_szName,
//													 m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsConvertItem),
//													 m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsConvertItem),
//													 m_pcsAgpmItemConvert->GetNumSocket(pcsConvertItem));
//			}
//			else
//			{
//				sprintf(szItemInfoNew, "%s [%d/%d]", ((AgpdItemTemplate *) pcsConvertItem->m_pcsItemTemplate)->m_szName,
//													 m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsConvertItem),
//													 m_pcsAgpmItemConvert->GetNumSocket(pcsConvertItem));
//			}
//
//			sprintf(szBuffer, szMessage, szItemInfoOld, szItemInfoNew);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RESULT_FAILED:
//		{
//			sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICAL_KEEPCURRENT));
//		}
//		break;
//
//	case AGPDITEMCONVERT_RESULT_FAILED_AND_INIT:
//		{
//			sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICAL_INITIALIZE));
//		}
//		break;
//
//	case AGPDITEMCONVERT_RESULT_FAILED_AND_DESTROY:
//		{
//			sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICAL_DESTROY));
//		}
//		break;
//
//	default:
//		{
//			CHAR	*szMessage	= m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICAL);
//			if (!szMessage)
//				return FALSE;
//
//			sprintf(szBuffer, szMessage, 1);	// 1은 개조에 필요한 강화축성제 갯수이다.
//		}
//		break;
//	}
//
//	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
//
//	return TRUE;
//}

BOOL AgcmUIEventItemConvert::UpdatePhysicalConvertConfirm(AgcdUIControl *pcsEditControl, AgpdItem *pcsConvertItem)
{
	if (!pcsEditControl || !pcsConvertItem)
		return FALSE;

	CHAR	*szMessage	= m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICAL_CONFIRM);
	if (!szMessage)
		return FALSE;

	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetLineDelimiter(m_pcsAgcmUIManager2->GetLineDelimiter());

	CHAR	szBuffer[256];
	ZeroMemory(szBuffer, sizeof(CHAR) * 256);

	CHAR	szItemInfoOld[64];
	CHAR	szItemInfoNew[64];

	ZeroMemory(szItemInfoOld, sizeof(CHAR) * 64);
	ZeroMemory(szItemInfoNew, sizeof(CHAR) * 64);

	if (m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsConvertItem) > 0)
	{
		sprintf(szItemInfoOld, "%s +%d [%d/%d]", ((AgpdItemTemplate *) pcsConvertItem->m_pcsItemTemplate)->m_szName,
											 m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsConvertItem),
											 m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsConvertItem),
											 m_pcsAgpmItemConvert->GetNumSocket(pcsConvertItem));
	}
	else
	{
		sprintf(szItemInfoOld, "%s [%d/%d]", ((AgpdItemTemplate *) pcsConvertItem->m_pcsItemTemplate)->m_szName,
											 m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsConvertItem),
											 m_pcsAgpmItemConvert->GetNumSocket(pcsConvertItem));
	}

	if (m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsConvertItem) + 1 > 0)
	{
		sprintf(szItemInfoNew, "%s +%d [%d/%d]", ((AgpdItemTemplate *) pcsConvertItem->m_pcsItemTemplate)->m_szName,
											 m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsConvertItem) + 1,
											 m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsConvertItem),
											 m_pcsAgpmItemConvert->GetNumSocket(pcsConvertItem));
	}
	else
	{
		sprintf(szItemInfoNew, "%s [%d/%d]", ((AgpdItemTemplate *) pcsConvertItem->m_pcsItemTemplate)->m_szName,
											 m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsConvertItem),
											 m_pcsAgpmItemConvert->GetNumSocket(pcsConvertItem));
	}

	sprintf(szBuffer, szMessage, szItemInfoOld, szItemInfoNew);

	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBResultPhysicalConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;
	AgpdItem				*pcsItem			= (AgpdItem *)					pData;
	AgpdItemConvertResult	*peResult			= (AgpdItemConvertResult *)		pCustData;

	pThis->m_bIsRequestConvert	= FALSE;

	pThis->m_lConvertResult	= *peResult;

	pThis->SetPhysicalResult(*peResult);

	switch (*peResult) {
	case AGPDITEMCONVERT_RESULT_SUCCESS:
		pThis->PlayEffectSuccess();
		pThis->RefreshUserDataConvertItem();
		break;

	case AGPDITEMCONVERT_RESULT_IS_ALREADY_FULL:
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPhysicalConvertAlreadyFull);
		break;

	case AGPDITEMCONVERT_RESULT_IS_EGO_ITEM:
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPhysicalConvertImproperItem);
		break;

	case AGPDITEMCONVERT_RESULT_INVALID_CATALYST:
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventErrInvalidCatalyst);
		break;

	case AGPDITEMCONVERT_RESULT_FAILED_AND_DESTROY:
		pThis->m_pcsConvertItem	= NULL; 
	case AGPDITEMCONVERT_RESULT_FAILED:
	case AGPDITEMCONVERT_RESULT_FAILED_AND_INIT:
	case AGPDITEMCONVERT_RESULT_FAILED_AND_INIT_SAME:
		pThis->PlayEffectFailed();
		pThis->RefreshUserDataConvertItem();
		break;
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::SetPhysicalResult(AgpdItemConvertResult eResult)
{
	if (!m_pcsPhysicalEditControl)
		return FALSE;

	CHAR	szBuffer[256];
	ZeroMemory(szBuffer, sizeof(CHAR) * 256);

	AcUIEdit *pcsEdit	= (AcUIEdit *) m_pcsPhysicalEditControl->m_pcsBase;
	pcsEdit->SetLineDelimiter("\n");

	switch (eResult) {
	case AGPDITEMCONVERT_RESULT_SUCCESS:
		if (AP_SERVICE_AREA_WESTERN == g_eServiceArea)
		{
		sprintf(szBuffer,
				"<C10092288>%s %d\n<C16773320>%s",
				m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICAL_SUCCESS),
				m_pcsAgpmItemConvert->GetNumPhysicalConvert(m_pcsConvertItem),
				m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_PHYSICAL_COST_CATALYST));
		}
		else
		{
		sprintf(szBuffer,
				"<C10092288>%d %s\n<C16773320>%s",
				m_pcsAgpmItemConvert->GetNumPhysicalConvert(m_pcsConvertItem),
				m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICAL_SUCCESS),
				m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_PHYSICAL_COST_CATALYST));
		}
		break;

	case AGPDITEMCONVERT_RESULT_INVALID_ITEM:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_SOCKET_IMPROPER_ITEM),
											   m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_PHYSICAL_COST_CATALYST));
		break;

	case AGPDITEMCONVERT_RESULT_FAILED:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICAL_KEEPCURRENT),
											   m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_PHYSICAL_COST_CATALYST));
		break;

	case AGPDITEMCONVERT_RESULT_FAILED_AND_INIT_SAME:
		CHAR	szFailMessage[128];
		ZeroMemory(szFailMessage, sizeof(CHAR) * 128);

		sprintf(szFailMessage, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICAL_INITIALIZE_SAME), m_lPrevNumPhysicalConvert);

		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", szFailMessage,
											   m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_PHYSICAL_COST_CATALYST));
		break;

	case AGPDITEMCONVERT_RESULT_FAILED_AND_INIT:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICAL_INITIALIZE),
											   m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_PHYSICAL_COST_CATALYST));
		break;

	case AGPDITEMCONVERT_RESULT_FAILED_AND_DESTROY:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_PHYSICAL_DESTROY),
											   m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_PHYSICAL_COST_CATALYST));
		break;

	case AGPDITEMCONVERT_RESULT_IS_ALREADY_FULL:
		sprintf(szBuffer, "%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_PHYSICAL_ALREADY_FULL),
											   m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_PHYSICAL_COST_CATALYST));
		break;

	case AGPDITEMCONVERT_RESULT_INVALID_CATALYST:
	case AGPDITEMCONVERT_RESULT_IS_EGO_ITEM:
		sprintf(szBuffer, "%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_PHYSICAL_IMPROPER_ITEM),
											   m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_PHYSICAL_COST_CATALYST));
		break;

	default:
		sprintf(szBuffer, "%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_INIT_TITLE),
											   m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_PHYSICAL_COST_CATALYST));
		break;
	}

	pcsEdit->SetText(szBuffer);

	return TRUE;
}