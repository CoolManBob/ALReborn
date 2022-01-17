/******************************************************************************
Module:  AgcmEventManager.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 23
******************************************************************************/

#include "AgcmEventManager.h"

AgcmEventManager::AgcmEventManager()
{
	SetModuleName("AgcmEventManager");

	m_bSetupEventObject = FALSE;
}

AgcmEventManager::~AgcmEventManager()
{
}

BOOL	AgcmEventManager::OnAddModule		(						)
{
	m_pcsAgcmObject				= (AgcmObject *)		GetModule("AgcmObject");
	m_pcsAgcmCharacter			= (AgcmCharacter *)		GetModule("AgcmCharacter");
	m_pcsApmEventManager		= (ApmEventManager *)	GetModule("ApmEventManager");

	if (!m_pcsApmEventManager)
		return FALSE;

	if (!m_pcsApmEventManager->SetCallbackInitObject(CBInitEventObject, this))
		return FALSE;

	return TRUE;
}

BOOL	AgcmEventManager::OnInit			(						)
{
	return TRUE;
}

BOOL	AgcmEventManager::OnDestroy		(						)
{
	return TRUE;
}

BOOL	AgcmEventManager::RegisterEventCallback(ApdEventFunction eFunction, ApModuleDefaultCallBack fnCallback, PVOID pvClass)
{
	if (!fnCallback || !pvClass || eFunction < 0 || eFunction >= APDEVENT_MAX_FUNCTION)
		return FALSE;

	return SetCallback((INT16) eFunction, fnCallback, pvClass);
}

BOOL	AgcmEventManager::CheckEvent		( ApBase *pcsBase,	ApBase *pcsGenerator	)
{
	if (!pcsBase || !pcsGenerator)
		return FALSE;

	ApdEventAttachData	*pcsAttachData = m_pcsApmEventManager->GetEventData(pcsBase);

	bool				bHaveNPCDialog;
	INT32				lNPCDialogIndex;

	bHaveNPCDialog = false;
	lNPCDialogIndex = 0;

	if (!pcsAttachData)
		return FALSE;

	//NPC Dialog Event를 가지고 있는지 본다.
	for (int i = 0; i < pcsAttachData->m_unFunction; ++i)
	{
		if (pcsAttachData->m_astEvent[i].m_eFunction > APDEVENT_FUNCTION_NONE && pcsAttachData->m_astEvent[i].m_eFunction < APDEVENT_MAX_FUNCTION)
		{
			if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_NPCDAILOG )
			{
				lNPCDialogIndex = i;
				bHaveNPCDialog = true;
			}
		}
	}

	//NPC Dialog Event를 가지고있다면 실행해준다.
	if( bHaveNPCDialog )
	{
		if (EnumCallback(pcsAttachData->m_astEvent[lNPCDialogIndex].m_eFunction, &pcsAttachData->m_astEvent[lNPCDialogIndex], pcsGenerator))
			return TRUE;
	}
	//NPC Dialog Event를 가지고있지 않다면 그냥 다 불러준다.
	else
	{
		for (int i = 0; i < pcsAttachData->m_unFunction; ++i)
		{
			if (pcsAttachData->m_astEvent[i].m_eFunction > APDEVENT_FUNCTION_NONE && pcsAttachData->m_astEvent[i].m_eFunction < APDEVENT_MAX_FUNCTION)
			{
				if (EnumCallback(pcsAttachData->m_astEvent[i].m_eFunction, &pcsAttachData->m_astEvent[i], pcsGenerator))
					return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL	AgcmEventManager::CheckEvent		( ApBase *pcsBase,	ApBase *pcsGenerator, ApdEventFunction eFunction )
{
	if (!pcsBase || !pcsGenerator)
		return FALSE;

	ApdEventAttachData	*pcsAttachData = m_pcsApmEventManager->GetEventData(pcsBase);
	if (!pcsAttachData)
		return FALSE;

	for (int i = 0; i < pcsAttachData->m_unFunction; ++i)
	{
		if (pcsAttachData->m_astEvent[i].m_eFunction == eFunction )
		{
			if(EnumCallback(pcsAttachData->m_astEvent[i].m_eFunction, &pcsAttachData->m_astEvent[i], pcsGenerator))
				return TRUE;
		}
	}

	return FALSE;
}

BOOL	AgcmEventManager::CBInitEventObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventManager *	pThis = (AgcmEventManager *) pClass;
	AgpdCharacter *		pcsCharacter = (AgpdCharacter *) pData;
	ApdObject *			pcsObject = (ApdObject *) pData;

	if (!pThis->m_bSetupEventObject || !pData)
		return TRUE;

	switch (((ApBase *) pData)->m_eType)
	{
	case APBASE_TYPE_OBJECT:
		{
			if (pThis->m_pcsAgcmObject)
			{
				pThis->m_pcsAgcmObject->SetupObjectClump(pcsObject);
			}
		}
		break;

		/*
	case APBASE_TYPE_CHARACTER:
		{
			if (pThis->m_pcsAgcmCharacter)
			{
				pThis->m_pcsAgcmCharacter->InitCharacter(pcsCharacter);
			}
		}
		break;
		*/
	}

	return TRUE;
}