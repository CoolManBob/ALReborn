#include "AgcmUIEventItemConvert.h"

BOOL AgcmUIEventItemConvert::CBSetSocketConvert(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIEventItemConvert	*pThis				= (AgcmUIEventItemConvert *)	pClass;

	return pThis->InitSocketConvertUI();

//	pThis->RefreshUserDataConvertType();
//
//	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpdateConvertInfo);
//
//	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenConvertUI);
}

BOOL AgcmUIEventItemConvert::InitSocketConvertUI()
{
	m_eConvertType	= AGCMUI_ITEMCONVERT_TYPE_SOCKET;

	m_pcsAgpmGrid->Reset(&m_stGridSocket);

	m_llConvertCost	= 0;

	m_lConvertResult	= (-1);

	RefreshUserDataGridSocket();
	RefreshUserDataConvertItem();

	RecalcConvertCost(m_pcsConvertItem);

	SetSocketResult(AGPDITEMCONVERT_SOCKET_RESULT_NONE);

	// Display 데이타들을 세팅한다.
	//
	//
	//
	//
	//
	//

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CheckSocketConvert()
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

	AgpdItemConvertSocketResult	eResult	= m_pcsAgcmItemConvert->IsSocketConvertable(pcsConvertItem);

	switch (eResult) {
//	case AGPDITEMCONVERT_SOCKET_RESULT_IS_ALREADY_FULL:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventSocketConvertAlreadyFull);
//		}
//		break;
//
//	case AGPDITEMCONVERT_SOCKET_RESULT_IS_EGO_ITEM:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventSocketConvertImproperItem);
//		}
//		break;
//
//	case AGPDITEMCONVERT_SOCKET_RESULT_NOT_ENOUGH_MONEY:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventSocketConvertNotEnoughMoney);
//		}
//		break;
//
//	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED:
//		{
//		}
//		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS:
		{
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventUpdateConvertConfirm);

			m_pcsAgcmUIManager2->ThrowEvent(m_lEventConvertConfirm);
		}
		break;

	default:
		SetSocketResult(eResult);
		break;
	}

	return TRUE;
}

BOOL AgcmUIEventItemConvert::ThrowEventSocketConvertable(AgpdItemConvertSocketResult eResult)
{
	return SetSocketResult(eResult);

//	switch (eResult) {
//	case AGPDITEMCONVERT_SOCKET_RESULT_IS_ALREADY_FULL:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventSocketConvertAlreadyFull);
//		}
//		break;
//
//	case AGPDITEMCONVERT_SOCKET_RESULT_IS_EGO_ITEM:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventSocketConvertImproperItem);
//		}
//		break;
//
//	case AGPDITEMCONVERT_SOCKET_RESULT_NOT_ENOUGH_MONEY:
//		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventSocketConvertNotEnoughMoney);
//		}
//		break;
//
//	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED:
//		{
//		}
//		break;
//	}
//
//	return TRUE;
}

BOOL AgcmUIEventItemConvert::StartSocketConvert()
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

	AgpdItemConvertSocketResult	eResult	= m_pcsAgcmItemConvert->SocketConvert(pcsConvertItem);

	switch (eResult) {
	case AGPDITEMCONVERT_SOCKET_RESULT_IS_ALREADY_FULL:
		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventSocketConvertAlreadyFull);
			SetSocketResult(eResult);
		}
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_IS_EGO_ITEM:
		{
		}
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_NOT_ENOUGH_MONEY:
		{
//			m_pcsAgcmUIManager2->ThrowEvent(m_lEventSocketConvertNotEnoughMoney);
			SetSocketResult(eResult);
		}
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED:
		{
		}
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS:
		{
		}
		break;
	}

	return TRUE;
}

//BOOL AgcmUIEventItemConvert::UpdateSocketConvertInfo(AgcdUIControl *pcsEditControl)
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
//	case AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS:
//		{
//			CHAR	*szMessage	= m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_SOCKET_SUCCESS);
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
//			sprintf(szBuffer, szMessage, m_pcsAgpmItemConvert->GetNumSocket(pcsConvertItem) - 1, m_pcsAgpmItemConvert->GetNumSocket(pcsConvertItem));
//		}
//		break;
//
//	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED:
//		{
//			sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_SOCKET_KEEPCURRENT));
//		}
//		break;
//
//	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT:
//		{
//			sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_SOCKET_INITIALIZE));
//		}
//		break;
//
//	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_DESTROY:
//		{
//			sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_SOCKET_DESTROY));
//		}
//		break;
//
//	default:
//		{
//			sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_SOCKET));
//		}
//		break;
//	}
//
//	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
//
//	return TRUE;
//}

BOOL AgcmUIEventItemConvert::UpdateSocketConvertConfirm(AgcdUIControl *pcsEditControl, AgpdItem *pcsConvertItem)
{
	if (!pcsEditControl || !pcsConvertItem)
		return FALSE;

	CHAR	*szMessage	= m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_SOCKET_CONFIRM);
	if (!szMessage)
		return FALSE;

	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetLineDelimiter(m_pcsAgcmUIManager2->GetLineDelimiter());

	CHAR	szBuffer[256];
	ZeroMemory(szBuffer, sizeof(CHAR) * 256);

	CHAR	szItemInfoOld[32];

	ZeroMemory(szItemInfoOld, sizeof(CHAR) * 32);

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

	sprintf(szBuffer, szMessage, szItemInfoOld);

	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);

	return TRUE;
}

BOOL AgcmUIEventItemConvert::CBResultSocketConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmUIEventItemConvert		*pThis				= (AgcmUIEventItemConvert *)		pClass;
	AgpdItem					*pcsItem			= (AgpdItem *)						pData;
	AgpdItemConvertSocketResult	*peResult			= (AgpdItemConvertSocketResult *)	pCustData;

	pThis->RecalcConvertCost(pcsItem);

	pThis->m_lConvertResult	= *peResult;

	switch (*peResult) {
	case AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS:
		pThis->PlayEffectSuccess();
		pThis->RefreshUserDataConvertItem();
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_IS_ALREADY_FULL:
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSocketConvertAlreadyFull);
		//pThis->SetSocketResult(*peResult);
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_IS_EGO_ITEM:
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSocketConvertImproperItem);
		//pThis->SetSocketResult(*peResult);
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_NOT_ENOUGH_MONEY:
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSocketConvertNotEnoughMoney);
		//pThis->SetSocketResult(*peResult);
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_INVALID_ITEM:
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSocketConvertImproperItem);
		//pThis->SetSocketResult(*peResult);
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED:
	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT_SAME:
	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT:
	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_DESTROY:
		pThis->PlayEffectFailed();
		pThis->RefreshUserDataConvertItem();
		break;
	}

	pThis->SetSocketResult(*peResult);

	return TRUE;
}

BOOL AgcmUIEventItemConvert::RecalcConvertCost(AgpdItem *pcsItem)
{
	if (pcsItem)
	{
		m_llConvertCost	= m_pcsAgpmItemConvert->GetNextSocketCost(pcsItem);
		if (m_llConvertCost < 0)
			m_llConvertCost	= 0;
	}
	else
		m_llConvertCost	= 0;

//	RefreshUserDataConvertCost();

	INT64	llInvenMoney	= 0;
	m_pcsAgpmCharacter->GetMoney(m_pcsAgcmCharacter->GetSelfCharacter(), &llInvenMoney);

	if (llInvenMoney - m_llConvertCost < 0)
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventSocketConvertNotEnoughInvenMoney);
	else
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventSocketConvertEnoughInvenMoney);

	return RefreshUserDataConvertCost();
}

BOOL AgcmUIEventItemConvert::SetSocketResult(AgpdItemConvertSocketResult eResult)
{
	if (!m_pcsSocketEditControl)
		return FALSE;

	CHAR	szBuffer[256];
	ZeroMemory(szBuffer, sizeof(CHAR) * 256);

	AcUIEdit *pcsEdit	= (AcUIEdit *) m_pcsSocketEditControl->m_pcsBase;
	pcsEdit->SetLineDelimiter("\n");

	CHAR	szBuffer2[32];
	ZeroMemory(szBuffer2, sizeof(CHAR) * 32);

	if (m_pcsConvertItem)
	{
		/*
		sprintf(szBuffer2, "[%d/%d] -> [%d/%d]", 
							m_pcsAgpmItemConvert->GetNumConvertedSocket(m_pcsConvertItem),
							m_pcsAgpmItemConvert->GetNumSocket(m_pcsConvertItem),
							m_pcsAgpmItemConvert->GetNumConvertedSocket(m_pcsConvertItem),
							m_pcsAgpmItemConvert->GetNumSocket(m_pcsConvertItem) + 1);
		*/
	}

	switch (eResult) {
	case AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS:
		CHAR	szBuffer3[256];
		ZeroMemory(szBuffer3, sizeof(CHAR) * 256);
		sprintf(szBuffer3, "<C10092288>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_SOCKET_SUCCESS), szBuffer2);
		sprintf(szBuffer, szBuffer3, m_pcsAgpmItemConvert->GetNumSocket(m_pcsConvertItem));
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_IS_ALREADY_FULL:
		sprintf(szBuffer, "%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_SOCKET_ALREADY_FULL));
		break;

//	case AGPDITEMCONVERT_SOCKET_RESULT_IS_EGO_ITEM:
//		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_NOT_ENOUGH_MONEY:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_SOCKET_NOTENOUGH_MONEY), szBuffer2);
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_INVALID_ITEM:
		sprintf(szBuffer, "<C16711680>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_SOCKET_IMPROPER_ITEM));
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_SOCKET_KEEPCURRENT), szBuffer2);
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT_SAME:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_SOCKET_INITIALIZE_SAME), szBuffer2);
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_SOCKET_INITIALIZE), szBuffer2);
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_DESTROY:
		sprintf(szBuffer, "<C16711680>%s\n<C16773320>%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONV_SOCKET_DESTROY), szBuffer2);
		break;

	default:
		sprintf(szBuffer, "%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CONVERT_INIT_TITLE));
		break;
	}

	pcsEdit->SetText(szBuffer);

	return TRUE;
}