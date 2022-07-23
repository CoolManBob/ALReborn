#include "AgcmUIEventItemConvert.h"

BOOL AgcmUIEventItemConvert::CBSetRuneConvert(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;

	return pThis->InitRuneConvertUI();

//	pThis->RefreshUserDataConvertType();
//
//	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpdateConvertInfo);
//
//	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenConvertUI);
}

BOOL AgcmUIEventItemConvert::InitRuneConvertUI()
{
	m_eConvertType	= AGCMUI_ITEMCONVERT_TYPE_RUNE;

	m_pcsAgpmGrid->Reset(&m_stGridSocket);

	m_llConvertCost	= 0;

	m_lConvertResult	= (-1);

	RefreshUserDataGridSocket();
	RefreshUserDataConvertCost();
	RefreshUserDataConvertItem();

	SetRuneResult(AGPDITEMCONVERT_RUNE_RESULT_NONE);

	// Display 데이타들을 세팅한다.
	//
	//
	//
	//
	//
	//

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CheckRuneConvert()
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

	// 2. 기원석이 있는지 본다.
	AgpdGridItem	*pcsRuneGridItem		= m_pcsAgpmGrid->GetItem(&m_stGridSocket, 0, 0, 0);
	if (!pcsRuneGridItem)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidRune);
		return FALSE;
	}

	AgpdItem		*pcsRuneItem			= m_pcsAgpmItem->GetItem(pcsRuneGridItem->m_lItemID);
	if (!pcsRuneItem)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidRune);
		return FALSE;
	}

	AgpdItemConvertRuneResult	eResult	= m_pcsAgcmItemConvert->IsRuneConvertable(pcsConvertItem, pcsRuneItem);

	switch (eResult) {
//	case AGPDITEMCONVERT_RUNE_RESULT_INVALID_RUNE_ITEM:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidRune);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_FULL:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventRuneConvertNotEnoughSocket);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_EGO_ITEM:
//		{
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_CHAR_LEVEL:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrLowCharLevel);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_ITEM_LEVEL:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrLowItemLevel);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_IMPROPER_PART:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrImproperPart);
//		}
//		break;
//		
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_ANTI_CONVERT:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrAlreadyAntiConvert);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_FAILED:
//		{
//		}
//		break;

	case AGPDITEMCONVERT_RUNE_RESULT_SUCCESS:
		{
			strcpy(m_szRuneName, ((AgpdItemTemplate *) pcsRuneItem->m_pcsItemTemplate)->m_szName.c_str());

			m_pcsAgcmUIManager2->ThrowEvent(m_lEventUpdateConvertConfirm);

			m_pcsAgcmUIManager2->ThrowEvent(m_lEventConvertConfirm);
		}
		break;

	default:
		SetRuneResult(eResult);
		break;
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::ThrowEventRuneConvertable(AgpdItemConvertRuneResult eResult)
{
	return SetRuneResult(eResult);

//	switch (eResult) {
//	case AGPDITEMCONVERT_RUNE_RESULT_INVALID_RUNE_ITEM:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidRune);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_FULL:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventRuneConvertNotEnoughSocket);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_EGO_ITEM:
//		{
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_CHAR_LEVEL:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrLowCharLevel);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_ITEM_LEVEL:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrLowItemLevel);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_IMPROPER_PART:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrImproperPart);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_ANTI_CONVERT:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrAlreadyAntiConvert);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_FAILED:
//		{
//		}
//		break;
//	}
//	
//	return TRUE;
}

BOOL AgcmUIEventItemConvert::StartRuneConvert()
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

	// 2. 기원석이 있는지 본다.
	AgpdGridItem	*pcsRuneGridItem		= m_pcsAgpmGrid->GetItem(&m_stGridSocket, 0, 0, 0);
	if (!pcsRuneGridItem)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidRune);
		return FALSE;
	}

	AgpdItem		*pcsRuneItem			= m_pcsAgpmItem->GetItem(pcsRuneGridItem->m_lItemID);
	if (!pcsRuneItem)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidRune);
		return FALSE;
	}

	AgpdItemConvertRuneResult	eResult	= m_pcsAgcmItemConvert->RuneConvert(pcsConvertItem, pcsRuneItem);

	switch (eResult) {
	case AGPDITEMCONVERT_RUNE_RESULT_INVALID_RUNE_ITEM:
		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrInvalidRune);
			SetRuneResult(eResult);
		}
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_FULL:
		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrNotEnoughSocket);
			SetRuneResult(eResult);
		}
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_EGO_ITEM:
		{
		}
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_CHAR_LEVEL:
		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrLowCharLevel);
			SetRuneResult(eResult);
		}
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_ITEM_LEVEL:
		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrLowItemLevel);
			SetRuneResult(eResult);
		}
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_IMPROPER_PART:
		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrImproperPart);
			SetRuneResult(eResult);
		}
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_ANTI_CONVERT:
		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventErrAlreadyAntiConvert);
			SetRuneResult(eResult);
		}
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_FAILED:
		{
		}
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_SUCCESS:
		{
		}
		break;
	}

	return TRUE;
}

//BOOL AgcmUIEventItemConvert::UpdateRuneConvertInfo(AgcdUIControl *pcsEditControl)
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
//	case AGPDITEMCONVERT_RUNE_RESULT_SUCCESS:
//		{
//			CHAR	*szMessage	= m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_RUNE_SUCCESS);
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
//
//			ZeroMemory(szItemInfoOld, sizeof(CHAR) * 32);
//
//			if (m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsConvertItem) > 0)
//			{
//				sprintf(szItemInfoOld, "%s +%d [%d/%d]", ((AgpdItemTemplate *) pcsConvertItem->m_pcsItemTemplate)->m_szName,
//													 m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsConvertItem),
//													 m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsConvertItem) - 1,
//													 m_pcsAgpmItemConvert->GetNumSocket(pcsConvertItem));
//			}
//			else
//			{
//				sprintf(szItemInfoOld, "%s [%d/%d]", ((AgpdItemTemplate *) pcsConvertItem->m_pcsItemTemplate)->m_szName,
//													 m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsConvertItem) - 1,
//													 m_pcsAgpmItemConvert->GetNumSocket(pcsConvertItem));
//			}
//
//			sprintf(szBuffer, szMessage, szItemInfoOld, m_szRuneName);
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_FAILED:
//		{
//			sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_RUNE_KEEPCURRENT));
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT:
//		{
//			sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_RUNE_INITIALIZE));
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_DESTROY:
//		{
//			sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_RUNE_DESTROY));
//		}
//		break;
//
//	default:
//		{
//			sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_RUNE));
//		}
//		break;
//	}
//
//	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
//
//	return TRUE;
//}

BOOL AgcmUIEventItemConvert::UpdateRuneConvertConfirm(AgcdUIControl *pcsEditControl, AgpdItem *pcsConvertItem)
{
	if (!pcsEditControl || !pcsConvertItem)
		return FALSE;

	CHAR	*szMessage	= m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_RUNE_CONFIRM);
	if (!szMessage)
		return FALSE;

	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetLineDelimiter(m_pcsAgcmUIManager2->GetLineDelimiter());

	CHAR	szBuffer[512];
	ZeroMemory(szBuffer, sizeof(CHAR) * 512);

	CHAR	szItemInfoOld[64];

	ZeroMemory(szItemInfoOld, sizeof(CHAR) * 64);

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

	sprintf(szBuffer, szMessage, szItemInfoOld, m_szRuneName);

	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBResultRuneConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmUIEventItemConvert		*pThis				= (AgcmUIEventItemConvert *)	pClass;
	AgpdItem					*pcsItem			= (AgpdItem *)					pData;
	AgpdItemConvertRuneResult	*peResult			= (AgpdItemConvertRuneResult *)	pCustData;

	pThis->m_lConvertResult	= *peResult;

	pThis->SetRuneResult(*peResult);

	switch (*peResult) {
	case AGPDITEMCONVERT_RUNE_RESULT_SUCCESS:
		pThis->PlayEffectSuccess();
		pThis->RefreshUserDataConvertItem();
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_INVALID_RUNE_ITEM:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventErrInvalidRune);
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_FULL:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventErrNotEnoughSocket);
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_EGO_ITEM:
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_CHAR_LEVEL:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventErrLowCharLevel);
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_ITEM_LEVEL:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventErrLowItemLevel);
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_IMPROPER_PART:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventErrImproperPart);
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_ANTI_CONVERT:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventErrAlreadyAntiConvert);
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_FAILED:
	case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT:
	case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT_SAME:
	case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_DESTROY:
		pThis->PlayEffectFailed();
		pThis->RefreshUserDataConvertItem();
		break;
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::SetRuneResult(AgpdItemConvertRuneResult eResult)
{
	if (!m_pcsRuneEditControl)
		return FALSE;

	CHAR	szBuffer[256];
	ZeroMemory(szBuffer, sizeof(CHAR) * 256);

	AcUIEdit *pcsEdit	= (AcUIEdit *) m_pcsRuneEditControl->m_pcsBase;
	pcsEdit->SetLineDelimiter("\n");

	switch (eResult) {
	case AGPDITEMCONVERT_RUNE_RESULT_SUCCESS:
		sprintf(szBuffer, "<C10092288>%s %s\n<C16773320>%s", m_szRuneName,
														  m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_RUNE_SUCCESS),
														  m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_INVALID_RUNE_ITEM:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_INVALID_RUNE),
														  m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_INVALID_ITEM:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_SOCKET_IMPROPER_ITEM),
														  m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_FULL:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_ALREADY_FULL),
														  m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
		break;

//	case AGPDITEMCONVERT_RUNE_RESULT_IS_EGO_ITEM:
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_CHAR_LEVEL:
//		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_ITEM_LEVEL:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_LOW_ITEM_LEVEL),
														  m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_IMPROPER_PART:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_IMPROPER_ITEM),
														  m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_ANTI_CONVERT:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_ALREADY_ANTI_CONV),
														  m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_FAILED:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_RUNE_KEEPCURRENT),
														  m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT_SAME:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_RUNE_INITIALIZE_SAME),
														  m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_RUNE_INITIALIZE),
														  m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_DESTROY:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_RUNE_DESTROY),
														  m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
		break;

	default:
		{
			if (m_pcsConvertItem)
			{
				sprintf(szBuffer, "%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_NONE),
													   m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
			}
			else
			{
				sprintf(szBuffer, "%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_INIT_TITLE),
													   m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_RUNE_COST));
			}
		}
		break;
	}

	pcsEdit->SetText(szBuffer);

	return TRUE;
}