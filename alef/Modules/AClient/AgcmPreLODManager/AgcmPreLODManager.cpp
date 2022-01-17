#include "AgcmPreLODManager.h"
#include "AcuObject.h"
#include "ApModuleStream.h"

AgcmPreLODManager::AgcmPreLODManager()
{
	// 마고자 (2003-11-12 오전 11:42:46) : 문제있어서 추가함..
	SetModuleName("AgcmPreLODManager");
	EnableIdle( FALSE );

	m_pcsAgcmObject	= NULL;
//	m_pstDummyClump = NULL;
}

AgcmPreLODManager::~AgcmPreLODManager()
{
}

BOOL AgcmPreLODManager::OnAddModule()
{
	m_pcsAgpmCharacter = (AgpmCharacter *)(GetModule("AgpmCharacter"));
	if(!m_pcsAgpmCharacter)
	{
		OutputDebugString("AgcmPreLODManager::OnAddModule() Error (1) !!!\n");
		return FALSE;
	}

	m_pcsAgcmCharacter = (AgcmCharacter *)(GetModule("AgcmCharacter"));
	if(!m_pcsAgcmCharacter)
	{
		OutputDebugString("AgcmPreLODManager::OnAddModule() Error (1-1) !!!\n");
		return FALSE;
	}

	m_pcsAgpmItem = (AgpmItem *)(GetModule("AgpmItem"));
	if(!m_pcsAgpmItem)
	{
		OutputDebugString("AgcmPreLODManager::OnAddModule() Error (2) !!!\n");
		return FALSE;
	}

	m_pcsAgcmItem = (AgcmItem *)(GetModule("AgcmItem"));
	if(!m_pcsAgcmItem)
	{
		OutputDebugString("AgcmPreLODManager::OnAddModule() Error (2-1) !!!\n");
		return FALSE;
	}

	m_pcsApmObject = (ApmObject *)(GetModule("ApmObject"));
	if(!m_pcsApmObject)
	{
		OutputDebugString("AgcmPreLODManager::OnAddModule() Error (3) !!!\n");
		return FALSE;
	}

	m_pcsAgcmObject = (AgcmObject *)(GetModule("AgcmObject"));
	if(!m_pcsAgcmObject)
	{
		OutputDebugString("AgcmPreLODManager::OnAddModule() Error (3-1) !!!\n");
		return FALSE;
	}

	m_pcsAgcmLODManager = (AgcmLODManager *)(GetModule("AgcmLODManager"));
	if(!m_pcsAgcmLODManager)
	{
		OutputDebugString("AgcmPreLODManager::OnAddModule() Error (3-2) !!!\n");
		return FALSE;
	}

	m_nCharPreLODAttachIndex = m_pcsAgpmCharacter->AttachCharacterTemplateData(this, sizeof(AgcdPreLOD), PreLODConstructCharCB, PreLODDestructCharCB);
	if(m_nCharPreLODAttachIndex < 0)
	{
		OutputDebugString("AgcmLODDlg::OnAddModule() Error (4) !!!\n");
		return FALSE;
	}

	m_nItemPreLODAttachIndex = m_pcsAgpmItem->AttachItemTemplateData(this, sizeof(AgcdPreLOD), PreLODConstructItemCB, PreLODDestructItemCB);
	if(m_nItemPreLODAttachIndex < 0)
	{
		OutputDebugString("AgcmLODDlg::OnAddModule() Error (4) !!!\n");
		return FALSE;
	}

	m_nObjectPreLODAttachIndex = m_pcsApmObject->AttachObjectTemplateData(this, sizeof(AgcdPreLOD), PreLODConstructObjtCB, PreLODDestructObjtCB);
	if(m_nObjectPreLODAttachIndex < 0)
	{
		OutputDebugString("AgcmLODDlg::OnAddModule() Error (4) !!!\n");
		return FALSE;
	}

	if(!AddStreamCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_CHARACTER, PreLODStreamReadCB, PreLODStreamWriteCB, this))
		return FALSE;

	if(!AddStreamCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_ITEM, PreLODStreamReadCB, PreLODStreamWriteCB, this))
		return FALSE;

	if(!AddStreamCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_OBJECT, PreLODStreamReadCB, PreLODStreamWriteCB, this))
		return FALSE;

	if(!m_pcsAgcmObject->SetCallbackLoadClump(ObjectLoadClumpCB, this))
		return FALSE;

	if(!m_pcsAgcmCharacter->SetCallbackLoadClump(CharacterLoadClumpCB, this))
		return FALSE;

	if(!m_pcsAgcmCharacter->SetCallbackLoadDefaultArmourClump(CharacterLoadDefaultArmourClumpCB, this))
		return FALSE;

	if(!m_pcsAgcmItem->SetCallbackLoadClump(ItemLoadClumpCB, this))
		return FALSE;

	return TRUE;
}
BOOL AgcmPreLODManager::OnInit()
{
//	m_pstDummyClump = m_pcsAgcmObject->LoadClump("Dummy.Dff");

	return TRUE;
}

BOOL AgcmPreLODManager::OnDestroy()
{
//	if(m_pstDummyClump)
//		RpClumpDestroy(m_pstDummyClump);

	return TRUE;
}

BOOL AgcmPreLODManager::PreLODConstructCharCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmPreLODManager		*pThis			= (AgcmPreLODManager *)(pClass);
	AgpdCharacterTemplate	*pcsTemplate	= (AgpdCharacterTemplate *)(pData);

	AgcdPreLOD				*pcsAgcdPreLOD = pThis->GetCharacterPreLOD(pcsTemplate);

	memset(pcsAgcdPreLOD, 0, sizeof(AgcdPreLOD));

	return TRUE;
}

BOOL AgcmPreLODManager::PreLODConstructItemCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmPreLODManager	*pThis			= (AgcmPreLODManager *)(pClass);
	AgpdItemTemplate	*pcsTemplate	= (AgpdItemTemplate *)(pData);

	AgcdPreLOD			*pcsAgcdPreLOD	= pThis->GetItemPreLOD(pcsTemplate);

	memset(pcsAgcdPreLOD, 0, sizeof(AgcdPreLOD));

	return TRUE;
}

BOOL AgcmPreLODManager::PreLODConstructObjtCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmPreLODManager		*pThis			= (AgcmPreLODManager *)(pClass);
	ApdObjectTemplate		*pcsTemplate	= (ApdObjectTemplate *)(pData);

	AgcdPreLOD				*pcsAgcdPreLOD	= pThis->GetObjectPreLOD(pcsTemplate);

	memset(pcsAgcdPreLOD, 0, sizeof(AgcdPreLOD));

	return TRUE;
}

BOOL AgcmPreLODManager::PreLODDestructCharCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmPreLODManager		*pThis			= (AgcmPreLODManager *)(pClass);
	AgpdCharacterTemplate	*pstTemplate	= (AgpdCharacterTemplate *)(pData);

	AgcdPreLOD				*pstAgcdPreLOD	= pThis->GetCharacterPreLOD(pstTemplate);

	return pThis->m_csPreLODAdmin.RemoveAllPreLODData(pstAgcdPreLOD);
}

BOOL AgcmPreLODManager::PreLODDestructItemCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmPreLODManager		*pThis			= (AgcmPreLODManager *)(pClass);
	AgpdItemTemplate		*pstTemplate	= (AgpdItemTemplate *)(pData);

	AgcdPreLOD				*pstAgcdPreLOD	= pThis->GetItemPreLOD(pstTemplate);

	return pThis->m_csPreLODAdmin.RemoveAllPreLODData(pstAgcdPreLOD);
}

BOOL AgcmPreLODManager::PreLODDestructObjtCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmPreLODManager		*pThis			= (AgcmPreLODManager *)(pClass);
	ApdObjectTemplate		*pstTemplate	= (ApdObjectTemplate *)(pData);

	AgcdPreLOD				*pstAgcdPreLOD	= pThis->GetObjectPreLOD(pstTemplate);

	return pThis->m_csPreLODAdmin.RemoveAllPreLODData(pstAgcdPreLOD);
}

AgcdPreLOD *AgcmPreLODManager::GetPreLOD(ApBase *pBaseTemplate)
{
	AgcdPreLOD	*pcsPreLOD = NULL;

	switch (pBaseTemplate->m_eType)
	{
	case APBASE_TYPE_CHARACTER_TEMPLATE:
		{
			pcsPreLOD = GetCharacterPreLOD((AgpdCharacterTemplate *)(pBaseTemplate));
		}
		break;

	case APBASE_TYPE_ITEM_TEMPLATE:
		{
			pcsPreLOD = GetItemPreLOD((AgpdItemTemplate *)(pBaseTemplate));
		}
		break;

	case APBASE_TYPE_OBJECT_TEMPLATE:
		{
			pcsPreLOD = GetObjectPreLOD((ApdObjectTemplate *)(pBaseTemplate));
		}
		break;
	}

	return pcsPreLOD;
}

AgcdPreLOD *AgcmPreLODManager::GetCharacterPreLOD(AgpdCharacterTemplate *pcsTemplate)
{
	return (AgcdPreLOD *)(m_pcsAgpmCharacter->GetAttachedModuleData(m_nCharPreLODAttachIndex, (void *)(pcsTemplate)));
}

AgcdPreLOD	*AgcmPreLODManager::GetItemPreLOD(AgpdItemTemplate *pcsAgpdItemTemplate)
{
	return (AgcdPreLOD *)(m_pcsAgpmItem->GetAttachedModuleData(m_nItemPreLODAttachIndex, (void *)(pcsAgpdItemTemplate)));
}

AgcdPreLOD	*AgcmPreLODManager::GetObjectPreLOD(ApdObjectTemplate *pcsApdObjectTemplate)
{
	return (AgcdPreLOD *)(m_pcsApmObject->GetAttachedModuleData(m_nObjectPreLODAttachIndex, (void *)(pcsApdObjectTemplate)));
}

AgpdCharacterTemplate *AgcmPreLODManager::GetCharacterTemplate(AgcdPreLOD *pcsAgcdPreLOD)
{
	return (AgpdCharacterTemplate *)(m_pcsAgpmCharacter->GetParentModuleData(m_nCharPreLODAttachIndex, (void *)(pcsAgcdPreLOD)));
}

AgpdItemTemplate *AgcmPreLODManager::GetItemTemplate(AgcdPreLOD *pcsAgcdPreLOD)
{
	return (AgpdItemTemplate *)(m_pcsAgpmItem->GetParentModuleData(m_nItemPreLODAttachIndex, (void *)(pcsAgcdPreLOD)));
}

ApdObjectTemplate *AgcmPreLODManager::GetObjectTemplate(AgcdPreLOD *pcsAgcdPreLOD)
{
	return (ApdObjectTemplate *)(m_pcsApmObject->GetParentModuleData(m_nObjectPreLODAttachIndex, (void *)(pcsAgcdPreLOD)));
}

AgcdPreLODData *AgcmPreLODManager::GetPreLODData(AgcdPreLOD *pstLOD, INT32 lIndex, BOOL bAdd)
{
	AgcdPreLODData *pstData = m_csPreLODAdmin.GetPreLODData(pstLOD, lIndex);
	if((!pstData) && (bAdd))
	{
		pstData = m_csPreLODAdmin.AddPreLODData(pstLOD);
		if((!pstData) || (lIndex != pstData->m_lIndex))
			return NULL;
	}

	return pstData;
}

BOOL AgcmPreLODManager::PreLODStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgcmPreLODManager	*pThis			= (AgcmPreLODManager *)(pClass);
	AgcdPreLOD			*pcsAgcdPreLOD	= (AgcdPreLOD *)(pData);

	const CHAR	*szValueName;
	CHAR		szValue[256];

	CHAR		szTemp[256];
	INT32		lTemp1, lTemp2, lTemp3;

	AgcdPreLODData *pstData;

	while(pStream->ReadNextValue())
	{
		szValueName	= pStream->GetValueName();

		if(!strncmp(szValueName, AGCM_PRELOD_STREAM_TYPE_PRELOD_DFF_NAME, strlen(AGCM_PRELOD_STREAM_TYPE_PRELOD_DFF_NAME)))
		{
			pStream->GetValue(szValue, 256);

			INT32	lCount = 0, lIndex = 0;
			for(lIndex = 0; lIndex < 256; ++lIndex)
			{
				if(szValue[lIndex] == ':')
				{
					++lCount;

					if(lCount == 2)
					{
						lCount = lIndex + 1;
						break;
					}
				}
			}

			for(lIndex = 0; lIndex < 256; ++lIndex)
			{
				if(szValue[lCount + lIndex] == ':')
				{
					szTemp[lIndex] = '\0';
					break;
				}

				szTemp[lIndex] = szValue[lCount + lIndex];
			}
			
			sscanf(szValue, "%d:%d", &lTemp1, &lTemp2);

			pstData = pThis->GetPreLODData(pcsAgcdPreLOD, lTemp1);
			if(!pstData)
				return FALSE;

			strcpy(pstData->m_aszData[lTemp2], szTemp);
		}
		else if(!strncmp(szValueName, AGCM_PRELOD_STREAM_TYPE_PRELOD_USEATOMICINDEX, strlen(AGCM_PRELOD_STREAM_TYPE_PRELOD_USEATOMICINDEX)))
		{
			pStream->GetValue(szValue, 256);
			
			sscanf(szValue, "%d:%d", &lTemp1, &lTemp2);

			pstData = pThis->GetPreLODData(pcsAgcdPreLOD, lTemp1);
			if(!pstData)
				return FALSE;

			pstData->m_bUseAtomicIndex = lTemp2;
		}
		else if(!strncmp(szValueName, AGCM_PRELOD_STREAM_TYPE_PRELOD_IS_BILLBOARD, strlen(AGCM_PRELOD_STREAM_TYPE_PRELOD_IS_BILLBOARD)))
		{
			pStream->GetValue(szValue, 256);
			
			sscanf(szValue, "%d:%d:%d", &lTemp1, &lTemp2, &lTemp3);

			pstData = pThis->GetPreLODData(pcsAgcdPreLOD, lTemp1);
			if(!pstData)
				return FALSE;

			pstData->m_stBillboard.isbill[lTemp2] = lTemp3;
		}
		else if(!strncmp(szValueName, AGCM_PRELOD_STREAM_TYPE_PRELOD_LEVEL, strlen(AGCM_PRELOD_STREAM_TYPE_PRELOD_LEVEL)))
		{
			pStream->GetValue(szValue, 256);
			
			sscanf(szValue, "%d:%d", &lTemp1, &lTemp2);

			pstData = pThis->GetPreLODData(pcsAgcdPreLOD, lTemp1);
			if(!pstData)
				return FALSE;

			pstData->m_stBillboard.num = lTemp2;
		}
		else if(!strncmp(szValueName, AGCM_PRELOD_STREAM_TYPE_PRELOD_TRANSFORM_GEOM, strlen(AGCM_PRELOD_STREAM_TYPE_PRELOD_TRANSFORM_GEOM)))
		{
			pStream->GetValue(szValue, 256);

			sscanf(szValue, "%d:%d:%d", &lTemp1, &lTemp2, &lTemp3);

			pstData = pThis->GetPreLODData(pcsAgcdPreLOD, lTemp1);
			if(!pstData)
				return FALSE;

			pstData->m_abTransformGeom[lTemp2] = lTemp3;
		}
	}

	return TRUE;
}

BOOL AgcmPreLODManager::PreLODStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	CHAR	szValueName[256];
	CHAR	szValue[256];
	BOOL	bSet = FALSE;

	if((!pClass) || (!pData))
		return FALSE;

	AgcmPreLODManager	*pcsThis		= (AgcmPreLODManager *)(pClass);
	AgcdPreLOD			*pstAgcdPreLOD	= (AgcdPreLOD *)(pData);
//	AgcdPreLODList	*pstList		= pstAgcdPreLOD->m_pstList;

//	while(pstList)
	AgcdPreLODData	*pcsData;
	for(INT32 lGroupIndex = 0;; ++lGroupIndex)
	{
		pcsData = pcsThis->m_csPreLODAdmin.GetPreLODData(pstAgcdPreLOD, lGroupIndex);
		if(!pcsData)
			break;

		for(INT32 lLODIndex = 1; lLODIndex < AGPDLOD_MAX_NUM; ++lLODIndex)
		{
			if(strcmp(pcsData->m_aszData[lLODIndex], "")) // 같지 않으면...
			{
				sprintf(szValueName, "%s%d%d", AGCM_PRELOD_STREAM_TYPE_PRELOD_DFF_NAME, pcsData->m_lIndex, lLODIndex);
				sprintf(szValue, "%d:%d:%s:", pcsData->m_lIndex, lLODIndex, pcsData->m_aszData[lLODIndex]);
				pStream->WriteValue(szValueName, szValue);

				if(pcsData->m_stBillboard.isbill[lLODIndex])
				{
					sprintf(szValueName, "%s%d%d", AGCM_PRELOD_STREAM_TYPE_PRELOD_IS_BILLBOARD, pcsData->m_lIndex, lLODIndex);
					sprintf(szValue, "%d:%d:%d:", pcsData->m_lIndex, lLODIndex, pcsData->m_stBillboard.isbill[lLODIndex]);
					pStream->WriteValue(szValueName, szValue);
				}

				if(pcsData->m_abTransformGeom[lLODIndex])
				{
					sprintf(szValueName, "%s%d%d", AGCM_PRELOD_STREAM_TYPE_PRELOD_TRANSFORM_GEOM, pcsData->m_lIndex, lLODIndex);
					sprintf(szValue, "%d:%d:%d:", pcsData->m_lIndex, lLODIndex, pcsData->m_abTransformGeom[lLODIndex]);
					pStream->WriteValue(szValueName, szValue);
				}

				bSet = TRUE;
			}
		}

		if(bSet)
		{
			sprintf(szValueName, "%s%d", AGCM_PRELOD_STREAM_TYPE_PRELOD_LEVEL, pcsData->m_lIndex);
			sprintf(szValue, "%d:%d:", pcsData->m_lIndex, pcsData->m_stBillboard.num);
			pStream->WriteValue(szValueName, szValue);

			sprintf(szValueName, "%s%d", AGCM_PRELOD_STREAM_TYPE_PRELOD_USEATOMICINDEX, pcsData->m_lIndex);
			sprintf(szValue, "%d:%d:", pcsData->m_lIndex, pcsData->m_bUseAtomicIndex);
			pStream->WriteValue(szValueName, szValue);
		}

//		pstList = pstList->m_pstNext;
	}

	return TRUE;
}

BOOL AgcmPreLODManager::CharPreLODStreamRead(CHAR *szFile)
{
	ApModuleStream	csStream;

	if(!csStream.Open(szFile))
	{
		return FALSE;
	}

	INT16		nTID;
	INT16		nNumKeys = csStream.GetNumSections();
	const CHAR *pszSectionName;

	for(INT16 i = 0; i < nNumKeys; ++i)
	{
		pszSectionName	= csStream.ReadSectionName(i);
		nTID			= atoi(pszSectionName);

		AgpdCharacterTemplate *pcsTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(nTID);
		if(!pcsTemplate)
		{
			OutputDebugString("AgcmPreLODManager::CharLodListStreamRead() Error (1) !!!\n");
			continue;
		}

		AgcdPreLOD *pcsAgcdPreLOD = GetCharacterPreLOD(pcsTemplate);
		if(!pcsAgcdPreLOD)
		{
			OutputDebugString("AgcmPreLODManager::CharLodListStreamRead() Error (2) !!!\n");
			return FALSE;
		}

		if(!csStream.EnumReadCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_CHARACTER, pcsAgcdPreLOD, this))
		{
			OutputDebugString("AgcmPreLODManager::CharLodListStreamRead() Error (3) !!!\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmPreLODManager::CharPreLODStreamWrite(CHAR *szFile)
{
	ApModuleStream			csStream;
	CHAR					szTemp[256];
	AgpdCharacterTemplate	*pcsTemplate;

	INT32 lIndex = 0;
	for(pcsTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
	{
		AgcdPreLOD *pcsAgcdPreLOD = GetCharacterPreLOD(pcsTemplate);
		if(!pcsAgcdPreLOD)
		{
			OutputDebugString("AgcmPreLODManager::CharLodListStreamWrite() Error (1) !!!\n");
			return FALSE;
		}

		sprintf(szTemp, "%d", pcsTemplate->m_lID);
		csStream.SetSection(szTemp);

		if(!csStream.EnumWriteCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_CHARACTER, pcsAgcdPreLOD, this))
		{
			OutputDebugString("AgcmPreLODManager::CharLodListStreamWrite() Error (2) !!!\n");
			return FALSE;
		}
	}

	csStream.Write(szFile);

	return TRUE;
}

BOOL AgcmPreLODManager::ItemPreLODStreamRead(CHAR *szFile)
{
	ApModuleStream	csStream;

	if(!csStream.Open(szFile))
	{
		return FALSE;
	}

	INT16	nNumKeys = csStream.GetNumSections();

	for(INT16 i = 0; i < nNumKeys; ++i)
	{
		AgpdItemTemplate *pcsAgpdItemTemplate = m_pcsAgpmItem->GetItemTemplate(atoi(csStream.ReadSectionName(i)));
		if(!pcsAgpdItemTemplate)
		{
			OutputDebugString("AgcmPreLODManager::ItemLodListStreamRead() Error (1) !!!\n");
			//return FALSE;
			continue;
		}

		AgcdPreLOD *pcsAgcdPreLOD = GetItemPreLOD(pcsAgpdItemTemplate);
		if(!pcsAgcdPreLOD)
		{
			OutputDebugString("AgcmPreLODManager::ItemLodListStreamRead() Error (2) !!!\n");
			return FALSE;
		}

		if(!csStream.EnumReadCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_ITEM, pcsAgcdPreLOD, this))
		{
			OutputDebugString("AgcmPreLODManager::ItemLodListStreamRead() Error (3) !!!\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmPreLODManager::ItemPreLODStreamWrite(CHAR *szFile)
{
	ApModuleStream			csStream;
	CHAR					szTemp[256];
	AgpdItemTemplate		*pcsTemplate;

	for(AgpaItemTemplate::iterator it = m_pcsAgcmItem->GetAgpmItem()->csTemplateAdmin.begin(); it != m_pcsAgcmItem->GetAgpmItem()->csTemplateAdmin.end(); ++it)
	{
		pcsTemplate = it->second;

		AgcdPreLOD *pcsAgcdPreLOD = GetItemPreLOD(pcsTemplate);
		if(!pcsAgcdPreLOD)
		{
			OutputDebugString("AgcmPreLODManager::ItemLodListStreamWrite() Error (1) !!!\n");
			return FALSE;
		}

		sprintf(szTemp, "%d", pcsTemplate->m_lID);
		csStream.SetSection(szTemp);

		if(!csStream.EnumWriteCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_ITEM, pcsAgcdPreLOD, this))
		{
			OutputDebugString("AgcmPreLODManager::ItemLodListStreamWrite() Error (2) !!!\n");
			return FALSE;
		}
	}

	csStream.Write(szFile);

	return TRUE;
}

BOOL AgcmPreLODManager::ObjectPreLODStreamRead(CHAR *szFile)
{
	ApModuleStream	csStream;
	if(!csStream.Open(szFile))
	{
		return FALSE;
	}

	INT16	nNumKeys = csStream.GetNumSections();

	for(INT16 i = 0; i < nNumKeys; ++i)
	{
		INT32 lTID = atoi(csStream.ReadSectionName(i));

		ApdObjectTemplate *pcsApdObjectTemplate = m_pcsApmObject->GetObjectTemplate(lTID);
		if(!pcsApdObjectTemplate)
		{
			OutputDebugString("AgcmPreLODManager::ObjectLodListStreamRead() Error (1) !!!\n");
			continue;
		}

		AgcdPreLOD *pcsAgcdPreLOD = GetObjectPreLOD(pcsApdObjectTemplate);
		if(!pcsAgcdPreLOD)
		{
			OutputDebugString("AgcmPreLODManager::ObjectLodListStreamRead() Error (2) !!!\n");
			return FALSE;
		}

		if(!csStream.EnumReadCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_OBJECT, pcsAgcdPreLOD, this))
		{
			OutputDebugString("AgcmPreLODManager::ObjectLodListStreamRead() Error (3) !!!\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmPreLODManager::ObjectPreLODStreamWrite(CHAR *szFile)
{
	ApModuleStream			csStream;
	CHAR					szTemp[256];
	ApdObjectTemplate		*pcsTemplate;

	INT32 lIndex = 0;
	for(pcsTemplate = m_pcsApmObject->GetObjectTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = m_pcsApmObject->GetObjectTemplateSequence(&lIndex))
	{
		AgcdPreLOD *pcsAgcdPreLOD = GetObjectPreLOD(pcsTemplate);
		if(!pcsAgcdPreLOD)
		{
			OutputDebugString("AgcmPreLODManager::ObjectLodListStreamWrite() Error (1) !!!\n");
			return FALSE;
		}

		sprintf(szTemp, "%d", pcsTemplate->m_lID);
		csStream.SetSection(szTemp);

		if(!csStream.EnumWriteCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_OBJECT, pcsAgcdPreLOD, this))
		{
			OutputDebugString("AgcmPreLODManager::ObjectLodListStreamWrite() Error (2) !!!\n");
			return FALSE;
		}
	}

	csStream.Write(szFile);

	return TRUE;
}

BOOL AgcmPreLODManager::SetLOD(RpClump *pstClump, AgcdPreLODData *pcsData, CHAR *szDataPath, INT32 lPartID)
{
	// 마고자 (2004-03-03 오후 4:10:22) : 정승환 일단 LOD 막음. . 더미 오브젝트 클론 갯수 초과 버그때문.
//	if (m_pcsAgcmLODManager->m_bForceZeroLevel)
//		return TRUE; // skip!
	
 	if((!pstClump) || (!pcsData))
		return FALSE;

	// 빌보드의 갯수가 아니라 LOD의 갯수이다.
	if(!pcsData->m_stBillboard.num)
		return TRUE; // SKIP 한다!

	CHAR	szLODPathName[256];
//	CHAR	szDummyPathName[256];
//	strcpy(szDummyPathName, ".\\Object\\Dummy.dff");

	RpClump	*pstDummyObject	= m_pcsAgcmObject->LoadClump(AGCM_PRELOD_DUMMY_OBJECT_NAME);
	if (!pstDummyObject)
		return FALSE;

//	if(!m_pcsAgcmLODManager->Initailize(pstClump, m_pstDummyClump, FALSE))
	if(!m_pcsAgcmLODManager->Initailize(pstClump, pstDummyObject, FALSE))
		return FALSE;

	for(INT32 lIndex = 1; lIndex < AGPDLOD_MAX_NUM; ++lIndex)
	{
		if((!strcmp(pcsData->m_aszData[lIndex], "")) || (!strcmp(pcsData->m_aszData[lIndex], "DUMMY")))
			break;

		sprintf(szLODPathName, "%s%s", szDataPath, pcsData->m_aszData[lIndex]);

/*		if (!m_pcsAgcmLODManager->MakeLODLevel(
			pstClump,							// 기본 클럼프
			szLODPathName,						// LOD용 클럼프 데이터 경로
			lIndex,								// LOD 인덱스
			m_pstDummyClump,					// 더미용 클럼프
			pcsData->m_abTransformGeom[lIndex],	// GEOMETRY를 TRANSFORM할지 여부
			FALSE,								// 다음 레벨의 GEOMETRY를 설정할지 여부
			lPartID								// 갑옷 클럼프에 쓰일 파트 아이디
			)																				)
		{
			return FALSE;
		}*/

		if (pcsData->m_stBillboard.isbill[lIndex] > 0)
		{
			if (!m_pcsAgcmLODManager->MakeLODLevel_Billboard(
				pstClump,						// 기본 클럼프
				szLODPathName,					// 빌보드 데이터 경로
				lIndex,							// LOD LEVEL
				pstDummyObject					// m_pstDummyClump					// 더미 클럼프
				)												)
			{
				return FALSE;
			}
		}
		else if (!m_pcsAgcmLODManager->MakeLODLevel(
			pstClump,							// 기본 클럼프
			szLODPathName,						// LOD용 클럼프 데이터 경로
			lIndex,								// LOD 인덱스
			pstDummyObject,						// m_pstDummyClump,					// 더미용 클럼프
			pcsData->m_abTransformGeom[lIndex],	// GEOMETRY를 TRANSFORM할지 여부
			FALSE,								// 다음 레벨의 GEOMETRY를 설정할지 여부
			lPartID								// 갑옷 클럼프에 쓰일 파트 아이디
			)																				)
		{
			return FALSE;
		}
	}

	RpClumpDestroy(pstDummyObject);

	AcuObject::SetClumpIsBillboard(pstClump, (PVOID)(&pcsData->m_stBillboard));

	return TRUE;
}

/*BOOL AgcmPreLODManager::SetLOD(ApdObjectTemplate *pstApdObjectTemplate, CHAR *szDataPath)
{
	if((!pstApdObjectTemplate) || (!szDataPath) || (!szDataPath[0]))
		return FALSE;

	AgcdPreLOD					*pstLOD					= GetObjectPreLOD(pstApdObjectTemplate);
	if(!pstLOD)
		return FALSE;

	AgcdObjectTemplate			*pstAgcdObjectTemplate	= m_pcsAgcmObject->GetTemplateData(pstApdObjectTemplate);
	if(!pstAgcdObjectTemplate)
		return FALSE;

	AgcdPreLODData				*pcsData;
	AgcdObjectTemplateGroupList	*pstList				= pstAgcdObjectTemplate->m_stGroup.m_pstList;
	while(pstList)
	{
		pcsData = m_csPreLODAdmin.GetPreLODData(pstLOD, pstList->m_csData.m_lIndex);
		if(!pcsData)
			return FALSE;

		if(!SetLOD(pstList->m_csData.m_pstClump, pcsData, szDataPath))
			return FALSE;

		pstList = pstList->m_pstNext;
	}

	return TRUE;
}*/

BOOL AgcmPreLODManager::ObjectLoadClumpCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pData) || (!pClass) || (!pCustData))
		return FALSE;

	AgcmPreLODManager			*pcsThis				= (AgcmPreLODManager *)(pClass);

	ApdObjectTemplate			*pstApdObjectTemplate	= (ApdObjectTemplate *)(pData);
	CHAR						*pszDataPath			= (CHAR *)(pCustData);

	AgcdPreLOD					*pstLOD					= pcsThis->GetObjectPreLOD(pstApdObjectTemplate);
	if(!pstLOD)
		return FALSE;

	AgcdObjectTemplate			*pstAgcdObjectTemplate	= pcsThis->m_pcsAgcmObject->GetTemplateData(pstApdObjectTemplate);
	if(!pstAgcdObjectTemplate)
		return FALSE;

	AgcdLODData					*pcsLODData;
	AgcdPreLODData				*pcsPreLODData;
	AgcdObjectTemplateGroupList	*pstList				= pstAgcdObjectTemplate->m_stGroup.m_pstList;
	while(pstList)
	{
		pcsLODData = pcsThis->m_pcsAgcmLODManager->m_csLODList.GetLODData(&pstAgcdObjectTemplate->m_stLOD, pstList->m_csData.m_lIndex);
		if((!pcsLODData) || (!pcsLODData->m_ulMaxLODLevel))
		{
			pstList = pstList->m_pstNext;
			continue;
		}

		pcsPreLODData = pcsThis->m_csPreLODAdmin.GetPreLODData(pstLOD, pstList->m_csData.m_lIndex);
		if(!pcsPreLODData)
			return FALSE;

		if(!pcsThis->SetLOD(pstList->m_csData.m_pstClump, pcsPreLODData, pszDataPath))
			return FALSE;

		pstList = pstList->m_pstNext;
	}

	return TRUE;
}

BOOL AgcmPreLODManager::CharacterLoadClumpCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pData) || (!pClass) || (!pCustData))
		return FALSE;

	AgcmPreLODManager	*pcsThis				= (AgcmPreLODManager *)(pClass);

	AgpdCharacterTemplate	*pcsTemplate	= (AgpdCharacterTemplate *)(pData);
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate	= pcsThis->m_pcsAgcmCharacter->GetTemplateData(pcsTemplate);
	CHAR					*pszDataPath				= (CHAR *)(pCustData);
	AgcdPreLOD				*pstPreLOD;

	if((!pstAgcdCharacterTemplate) || (!pstAgcdCharacterTemplate->m_pClump))
		return FALSE;

	if(RpClumpGetNumAtomics(pstAgcdCharacterTemplate->m_pClump) < 1)
		return TRUE; // skip

	if((!pstAgcdCharacterTemplate->m_stLOD.m_pstList) || (!pstAgcdCharacterTemplate->m_stLOD.m_pstList->m_csData.m_ulMaxLODLevel))
		return TRUE; // skip

	pstPreLOD = pcsThis->GetCharacterPreLOD(pcsTemplate);
	if(!pstPreLOD)
		return FALSE;

	//>@ 2005.3.28 gemani PreLOD가 없음 LOD단계를 0로 만들자..
	if(!(&pstPreLOD->m_pstList->m_csData))
	{
		AgcdLODList*	cur_LOD = pstAgcdCharacterTemplate->m_stLOD.m_pstList;
		while(cur_LOD)
		{
			for(int i=0;i<cur_LOD->m_csData.m_lHasBillNum;++i)
			{
				cur_LOD->m_csData.m_alBillInfo[i] = 0;
			}
			cur_LOD->m_csData.m_lHasBillNum = 0;
			cur_LOD->m_csData.m_ulMaxLODLevel = 0;

			cur_LOD = cur_LOD->m_pstNext;
		}

		return FALSE;
	}

	return pcsThis->SetLOD(pstAgcdCharacterTemplate->m_pClump, &pstPreLOD->m_pstList->m_csData, pszDataPath);
}

BOOL AgcmPreLODManager::CharacterLoadDefaultArmourClumpCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pData) || (!pClass) || (!pCustData))
		return FALSE;

	AgcmPreLODManager	*pcsThis				= (AgcmPreLODManager *)(pClass);

	AgpdCharacterTemplate	*pcsTemplate	= (AgpdCharacterTemplate *)(pData);
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate	= pcsThis->m_pcsAgcmCharacter->GetTemplateData(pcsTemplate);
	CHAR					*pszDataPath				= (CHAR *)(pCustData);
	AgcdPreLOD				*pstPreLOD;

	if((!pstAgcdCharacterTemplate) || (!pstAgcdCharacterTemplate->m_pClump))
		return FALSE;

	pstPreLOD = pcsThis->GetCharacterPreLOD(pcsTemplate);
	if(!pstPreLOD)
		return FALSE;

	if(pstPreLOD->m_lNum < 1)
		return TRUE; // skip

	if((!pstAgcdCharacterTemplate->m_stLOD.m_pstList) || (!pstAgcdCharacterTemplate->m_stLOD.m_pstList->m_csData.m_ulMaxLODLevel))
		return TRUE; // skip

	return pcsThis->SetLOD(pstAgcdCharacterTemplate->m_pDefaultArmourClump, &pstPreLOD->m_pstList->m_csData, pszDataPath);
}

BOOL AgcmPreLODManager::ItemLoadClumpCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pData) || (!pClass) || (!pCustData))
		return FALSE;

	AgcmPreLODManager		*pcsThis					= (AgcmPreLODManager *)(pClass);

	AgpdItemTemplate		*pcsAgpdItemTemplate		= (AgpdItemTemplate *)(pData);
	AgcdItemTemplate		*pstAgcdItemTemplate		= pcsThis->m_pcsAgcmItem->GetTemplateData(pcsAgpdItemTemplate);
	CHAR					*pszDataPath				= (CHAR *)(pCustData);
	AgcdPreLOD				*pstPreLOD;
	AgcdPreLODData			*pstPreLODData;
	AgcdLODData				*pstLODData;

	if((!pstAgcdItemTemplate) || (!pstAgcdItemTemplate->m_pcsPreData) || (!pstAgcdItemTemplate->m_pstBaseClump))
		return FALSE;

	pstPreLOD = pcsThis->GetItemPreLOD(pcsAgpdItemTemplate);
	if(!pstPreLOD)
		return FALSE;

	pstPreLODData = pcsThis->m_csPreLODAdmin.GetPreLODData(pstPreLOD, 0);
	if(!pstPreLODData)
		return FALSE;


	BOOL					bArmour						= FALSE;
	BOOL					bSetLOD						= TRUE;

	pstLODData = pcsThis->m_pcsAgcmLODManager->m_csLODList.GetLODData(&pstAgcdItemTemplate->m_stLOD, 0);
	if((pstLODData) && (pstLODData->m_ulMaxLODLevel))
	{
		if(	(pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP) &&
			(((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR)	)
		{
			bArmour = TRUE;

			if (pcsThis->m_pcsAgcmLODManager->IsSetLOD(pstAgcdItemTemplate->m_pstBaseClump))
				bSetLOD = FALSE;
		}
//			lPartID = ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nPart;

		if (bSetLOD)
		{
			if(!pcsThis->SetLOD(pstAgcdItemTemplate->m_pstBaseClump, pstPreLODData, pszDataPath))
				return FALSE;
		}
	}

	//if((!pstAgcdItemTemplate->m_szSecondDFFName[0]) || (!pstAgcdItemTemplate->m_pstSecondClump))
	if (	(!pstAgcdItemTemplate->m_pstSecondClump) ||
			(!pstAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName))
		return TRUE; // skip

	if (bArmour)
	{
		if (pcsThis->m_pcsAgcmLODManager->IsSetLOD(pstAgcdItemTemplate->m_pstSecondClump))
			return TRUE;
	}

	pstPreLODData = pcsThis->m_csPreLODAdmin.GetPreLODData(pstPreLOD, 1);
	if(!pstPreLODData)
		return FALSE;

	pstLODData = pcsThis->m_pcsAgcmLODManager->m_csLODList.GetLODData(&pstAgcdItemTemplate->m_stLOD, 1);
	if((!pstLODData) || (!pstLODData->m_ulMaxLODLevel))
		return TRUE; // skip

	return pcsThis->SetLOD(pstAgcdItemTemplate->m_pstSecondClump, pstPreLODData, pszDataPath);
}
