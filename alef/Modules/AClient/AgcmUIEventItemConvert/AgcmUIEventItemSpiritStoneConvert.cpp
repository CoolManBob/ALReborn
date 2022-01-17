#include "AgcmUIEventItemConvert.h"

BOOL AgcmUIEventItemConvert::CBResultSpiritStoneConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData || !pData)
		return FALSE;

	AgcmUIEventItemConvert				*pThis				= (AgcmUIEventItemConvert *)			pClass;
	AgpdItem							*pcsItem			= (AgpdItem *)							pData;
	AgpdItemConvertSpiritStoneResult	*peResult			= (AgpdItemConvertSpiritStoneResult *)	pCustData;

	switch (*peResult) {
	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS:
		pThis->PlayEffectSuccess(FALSE);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSpiritStoneConvertSuccess);
		pThis->RefreshUserDataConvertItem();
		break;

	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_FAILED:
		pThis->PlayEffectFailed(FALSE);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSpiritStoneConvertFail);
		pThis->RefreshUserDataConvertItem();
		break;

	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_INVALIE_SPIRITSTONE:
		break;

	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_IS_ALREADY_FULL:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSpiritStoneConvertAlreadyFull);
		break;

	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_IS_EGO_ITEM:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSpiritStoneConvertImproperItem);
		break;
		
	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_IMPROPER_ITEM:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSpiritStoneConvertImproperItem);
		break;
	}

	return TRUE;
}