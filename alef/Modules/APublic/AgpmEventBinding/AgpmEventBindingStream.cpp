//	AgpmEventBinding.cpp
//////////////////////////////////////////////////////////////////////

#include "AgpmEventBinding.h"
#include "ApModuleStream.h"

BOOL AgpmEventBinding::CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventBinding		*pThis				= (AgpmEventBinding *)		pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)				pData;
	ApModuleStream			*pStream			= (ApModuleStream *)		pCustData;

	if (!pcsEvent->m_pvData)
		return FALSE;

	pstAgpdBindingAttach	pstBindingAttach	= (pstAgpdBindingAttach)	pcsEvent->m_pvData;

	AgpdBinding				*pcsBinding			= pThis->GetBinding(pstBindingAttach->m_lBindingID);
	if (!pcsBinding)
		return FALSE;

	pstBindingAttach->m_pcsBinding				= pcsBinding;

	if (!pStream->WriteValue(AGPDBINDING_STREAM_BINDING_NAME, pcsBinding->m_szBindingName))
	{
		OutputDebugString("AgpmEventBinding::CBStreamWriteEvent() Error (1) !!!\n");
		return FALSE;
	}

	if (pcsBinding->m_szTownName && pcsBinding->m_szTownName[0])
	{
		if (!pStream->WriteValue(AGPDBINDING_STREAM_TOWN_NAME, pcsBinding->m_szTownName))
		{
			OutputDebugString("AgpmEventBinding::CBStreamWriteEvent() Error (2) !!!\n");
			return FALSE;
		}
	}

	if (!pStream->WriteValue(AGPDBINDING_STREAM_RADIUS, (INT32) pcsBinding->m_ulRadius))
	{
		OutputDebugString("AgpmEventBinding::CBStreamWriteEvent() Error (3) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPDBINDING_STREAM_BINDING_TYPE, (INT32) pcsBinding->m_eBindingType))
	{
		OutputDebugString("AgpmEventBinding::CBStreamWriteEvent() Error (4) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPDBINDING_STREAM_CHARACTER_RACE_TYPE, pcsBinding->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_RACE]))
	{
		OutputDebugString("AgpmEventBinding::CBStreamWriteEvent() Error (5) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPDBINDING_STREAM_CHARACTER_CLASS_TYPE, pcsBinding->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_CLASS]))
	{
		OutputDebugString("AgpmEventBinding::CBStreamWriteEvent() Error (6) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPDBINDING_STREAM_NAME_EVENT_END, 0))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventBinding::CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventBinding		*pThis				= (AgpmEventBinding *)		pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)				pData;
	ApModuleStream			*pStream			= (ApModuleStream *)		pCustData;

	if (!pcsEvent->m_pvData)
		return FALSE;

	pstAgpdBindingAttach	pstBindingAttach	= (pstAgpdBindingAttach)	pcsEvent->m_pvData;

	AgpdBinding				stBindingData;
	stBindingData.m_Mutex.Destroy();
	ZeroMemory(&stBindingData, sizeof(stBindingData));

	const CHAR				*szValueName		= NULL;

	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPDBINDING_STREAM_BINDING_NAME))
		{
			ZeroMemory(stBindingData.m_szBindingName, sizeof(CHAR) * (AGPDBINDING_MAX_NAME + 1));
			strncpy(stBindingData.m_szBindingName, (CHAR *) pStream->GetValue(), AGPDBINDING_MAX_NAME);
		}
		else if (!strcmp(szValueName, AGPDBINDING_STREAM_TOWN_NAME))
		{
			ZeroMemory(stBindingData.m_szTownName, sizeof(CHAR) * (AGPDBINDING_MAX_TOWN_NAME + 1));
			strncpy(stBindingData.m_szTownName, (CHAR *) pStream->GetValue(), AGPDBINDING_MAX_TOWN_NAME);
		}
		else if (!strcmp(szValueName, AGPDBINDING_STREAM_RADIUS))
		{
			pStream->GetValue((INT32 *) &stBindingData.m_ulRadius);
		}
		else if (!strcmp(szValueName, AGPDBINDING_STREAM_BINDING_TYPE))
		{
			pStream->GetValue((INT32 *) &stBindingData.m_eBindingType);
		}
		else if (!strcmp(szValueName, AGPDBINDING_STREAM_CHARACTER_RACE_TYPE))
		{
			pStream->GetValue((INT32 *) &stBindingData.m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_RACE]);
		}
		else if (!strcmp(szValueName, AGPDBINDING_STREAM_CHARACTER_CLASS_TYPE))
		{
			pStream->GetValue((INT32 *) &stBindingData.m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_CLASS]);
		}
		else if (!strcmp(szValueName, AGPDBINDING_STREAM_NAME_EVENT_END))
			break;
	}
	
	pThis->m_pcsApmEventManager->GetBasePos(pcsEvent, &stBindingData.m_stBasePos);

	AgpdBinding	*pcsBinding	= pThis->AddBinding(stBindingData.m_szBindingName,
												stBindingData.m_szTownName,
												&stBindingData.m_stBasePos,
												stBindingData.m_ulRadius,
												stBindingData.m_eBindingType);

	if (!pcsBinding)
		return FALSE;

	pcsBinding->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_RACE]		= stBindingData.m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_RACE];
	pcsBinding->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_CLASS]		= stBindingData.m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_CLASS];

	pstBindingAttach->m_lBindingID	= pcsBinding->m_lID;
	pstBindingAttach->m_pcsBinding	= pcsBinding;

	return TRUE;
}