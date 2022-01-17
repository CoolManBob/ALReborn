#include "stdafx.h"
#include "ModelTool.h"

BOOL CModelToolApp::CheckCharacterTemplateLODLevel()
{
	if(m_bExport)		return TRUE;

	INT32					lIndex = 0;
	AgpdCharacterTemplate	*ppTemplate;
	AgcdCharacterTemplate	*pcTemplate;
	AgcdLODData				*pcsLODData;
	AgcdPreLOD				*pstPreLOD;
	AgcdPreLODData			*pcsPreLODData;

	for(	ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex);
			ppTemplate;
			ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex)	)
	{
		pcTemplate = m_csAgcEngine.GetAgcmCharacterModule()->GetTemplateData(ppTemplate);
		if(!pcTemplate)
			return FALSE;

		pcsLODData = m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.GetLODData(&pcTemplate->m_stLOD, 0);
		if(!pcsLODData)
			continue;

		if(!pcsLODData->m_ulMaxLODLevel)
			continue;

		pstPreLOD = m_csAgcEngine.GetAgcmPreLODManagerModule()->GetCharacterPreLOD(ppTemplate);
		if(!pstPreLOD)
			return FALSE;

		pcsPreLODData = m_csAgcEngine.GetAgcmPreLODManagerModule()->GetPreLODData(pstPreLOD, 0, FALSE);
		if(!pcsPreLODData)
		{
			pcsLODData->m_ulMaxLODLevel = 0;
		}
		else
		{
			INT32 lCount;
			for(lCount = 1; lCount < AGPDLOD_MAX_NUM; ++lCount)
			{
				if(!strcmp(pcsPreLODData->m_aszData[lCount], ""))
					break;
			}

			pcsLODData->m_ulMaxLODLevel = (lCount == 1) ? (0) : (lCount);
		}
	}

	return TRUE;
}

BOOL CModelToolApp::CheckItemTemplateLODLevel()
{
	if(m_bExport)		return TRUE;

	INT32				lIndex = 0;
	AgpdItemTemplate	*ppTemplate;
	AgcdItemTemplate	*pcTemplate;
	AgcdLODData			*pcsLODData;
	AgcdPreLOD			*pstPreLOD;
	AgcdPreLODData		*pcsPreLODData;

	for(	ppTemplate = m_csAgcEngine.GetAgpmItemModule()->GetTemplateSequence(&lIndex);
			ppTemplate;
			ppTemplate = m_csAgcEngine.GetAgpmItemModule()->GetTemplateSequence(&lIndex)	)
	{
		pcTemplate = m_csAgcEngine.GetAgcmItemModule()->GetTemplateData(ppTemplate);
		if(!pcTemplate)
			return FALSE;

		pcsLODData = m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.GetLODData(&pcTemplate->m_stLOD, 0);
		if(!pcsLODData)
			continue;

		if(!pcsLODData->m_ulMaxLODLevel)
			continue;

		pstPreLOD = m_csAgcEngine.GetAgcmPreLODManagerModule()->GetItemPreLOD(ppTemplate);
		if(!pstPreLOD)
			return FALSE;

		pcsPreLODData = m_csAgcEngine.GetAgcmPreLODManagerModule()->GetPreLODData(pstPreLOD, 0, FALSE);
		if(!pcsPreLODData)
		{
			pcsLODData->m_ulMaxLODLevel = 0;
		}
		else
		{
			INT32 lCount;

			for(lCount = 1; lCount < AGPDLOD_MAX_NUM; ++lCount)
			{
				if(!strcmp(pcsPreLODData->m_aszData[lCount], ""))
					break;
			}

			pcsLODData->m_ulMaxLODLevel = (lCount == 1) ? (0) : (lCount);
		}
	}
//#endif
	return TRUE;
}

BOOL CModelToolApp::CheckObjectTemplateLODLevel()
{
	if(m_bExport)		return TRUE;

	INT32							lIndex = 0, lGroupIndex;
	ApdObjectTemplate				*ppTemplate;
	AgcdObjectTemplate				*pcTemplate;
	AgcdLODData						*pcsLODData;
	AgcdPreLOD						*pstPreLOD;
	AgcdPreLODData					*pcsPreLODData;
	AgcdObjectTemplateGroupData		*pcsGroupData;

	for(	ppTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplateSequence(&lIndex);
			ppTemplate;
			ppTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplateSequence(&lIndex)	)
	{
		pcTemplate = m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(ppTemplate);
		if(!pcTemplate)
			return FALSE;

		pstPreLOD = m_csAgcEngine.GetAgcmPreLODManagerModule()->GetObjectPreLOD(ppTemplate);
		if(!pstPreLOD)
			return FALSE;

		for(lGroupIndex = 0; ; ++lGroupIndex)
		{
			pcsGroupData = m_csAgcEngine.GetAgcmObjectModule()->GetObjectList()->GetObjectTemplateGroup(&pcTemplate->m_stGroup, lGroupIndex);
			if(!pcsGroupData)
				break;

			pcsLODData = m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.GetLODData(&pcTemplate->m_stLOD, lGroupIndex);
			if(!pcsLODData)
				continue;

			if(!pcsLODData->m_ulMaxLODLevel)
				continue;

			pcsPreLODData = m_csAgcEngine.GetAgcmPreLODManagerModule()->GetPreLODData(pstPreLOD, lGroupIndex, FALSE);
			if(!pcsPreLODData)
			{
				pcsLODData->m_ulMaxLODLevel = 0;
			}
			else
			{
				INT32 lCount;
				for(lCount = 1; lCount < AGPDLOD_MAX_NUM; ++lCount)
				{
					if(!strcmp(pcsPreLODData->m_aszData[lCount], ""))
						break;
				}

				pcsLODData->m_ulMaxLODLevel = (lCount == 1) ? (0) : (lCount);
			}
		}
	}

	return TRUE;
}

BOOL CModelToolApp::GenerateCharacterTemplateLODBoundary()
{
	INT32					lIndex = 0;
	AgpdCharacterTemplate	*ppTemplate;
	AgcdCharacterTemplate	*pcTemplate;
	AgcdLODData				*pcsLODData;

	for(	ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex);
			ppTemplate;
			ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex)	)
	{
		pcTemplate = m_csAgcEngine.GetAgcmCharacterModule()->GetTemplateData(ppTemplate);
		if(!pcTemplate)
			return FALSE;

		pcsLODData = m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.GetLODData(&pcTemplate->m_stLOD, 0);
		if(!pcsLODData)
			continue;

		if(!pcsLODData->m_ulMaxLODLevel)
			continue;

		pcsLODData->m_ulBoundary = (m_csAgcEngine.GetAgcmLODManagerModule()->GetStarndardDistance() / 100) * pcsLODData->m_ulMaxDistanceRatio;
	}

	return TRUE;
}

BOOL CModelToolApp::GenerateItemTemplateLODBoundary()
{
	INT32				lIndex = 0;
	AgpdItemTemplate	*ppTemplate;
	AgcdItemTemplate	*pcTemplate;
	AgcdLODData			*pcsLODData;

	for(	ppTemplate = m_csAgcEngine.GetAgpmItemModule()->GetTemplateSequence(&lIndex);
			ppTemplate;
			ppTemplate = m_csAgcEngine.GetAgpmItemModule()->GetTemplateSequence(&lIndex)	)
	{
		pcTemplate = m_csAgcEngine.GetAgcmItemModule()->GetTemplateData(ppTemplate);
		if(!pcTemplate)
			return FALSE;

		pcsLODData = m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.GetLODData(&pcTemplate->m_stLOD, 0);
		if(!pcsLODData)
			continue;

		if(!pcsLODData->m_ulMaxLODLevel)
			continue;

		pcsLODData->m_ulBoundary = (m_csAgcEngine.GetAgcmLODManagerModule()->GetStarndardDistance() / 100) * pcsLODData->m_ulMaxDistanceRatio;
	}

	return TRUE;
}

BOOL CModelToolApp::GenerateObjectTemplateLODBoundary()
{
	INT32							lIndex = 0, lGroupIndex;
	ApdObjectTemplate				*ppTemplate;
	AgcdObjectTemplate				*pcTemplate;
	AgcdLODData						*pcsLODData;
	AgcdObjectTemplateGroupData		*pcsGroupData;

	for(	ppTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplateSequence(&lIndex);
			ppTemplate;
			ppTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplateSequence(&lIndex)	)
	{
		pcTemplate = m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(ppTemplate);
		if(!pcTemplate)
			return FALSE;

		for(lGroupIndex = 0; ; ++lGroupIndex)
		{
			pcsGroupData = m_csAgcEngine.GetAgcmObjectModule()->GetObjectList()->GetObjectTemplateGroup(&pcTemplate->m_stGroup, lGroupIndex);
			if(!pcsGroupData)
				break;

			pcsLODData = m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.GetLODData(&pcTemplate->m_stLOD, lGroupIndex);
			if(!pcsLODData)
				continue;

			if(!pcsLODData->m_ulMaxLODLevel)
				continue;

			pcsLODData->m_ulBoundary = (m_csAgcEngine.GetAgcmLODManagerModule()->GetStarndardDistance() / 100) * pcsLODData->m_ulMaxDistanceRatio;
		}
	}

	return TRUE;
}