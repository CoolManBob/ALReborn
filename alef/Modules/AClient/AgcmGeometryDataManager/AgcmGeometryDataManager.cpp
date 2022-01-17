#include "AgcmGeometryDataManager.h"
#include "ApModuleStream.h"

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
AgcmGeometryDataAdmin::AgcmGeometryDataAdmin()
{
	m_unNumAllocGeom				= 0;
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
AcuGeometry *AgcmGeometryDataAdmin::AddGeometry(AgcdGeometryDataLink *pcsLink, AcuGeometry *pstGeom)
{
	AgcdGeometryData	*pcsCur;

	if (!pcsLink->m_pcsHead)
	{
		pcsLink->m_pcsHead	= new AgcdGeometryData();
		if (!pcsLink->m_pcsHead)
			return NULL;

		pcsCur				= pcsLink->m_pcsHead;
	}
	else
	{
		pcsCur				= pcsLink->m_pcsHead;

		while (pcsCur)
		{
			if (!pcsCur->m_pcsNext)
				break;

			pcsCur = pcsCur->m_pcsNext;
		}

		pcsCur->m_pcsNext	= new AgcdGeometryData();
		if (!pcsCur->m_pcsNext)
			return NULL;

		pcsCur				= pcsCur->m_pcsNext;
	}
	

	if (pstGeom)
	{
		memcpy(&pcsCur->m_stGeom, pstGeom, sizeof(AcuGeometry));
	}

	return &pcsCur->m_stGeom;
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
AcuGeometry *AgcmGeometryDataAdmin::AddGeometry(AgcdGeometryFrame *pcsFrame, UINT8 unKey, AcuGeometry *pstGeom)
{
	if (!pcsFrame->m_acsLink)
	{
		if (!CreateGeometryLink(pcsFrame))
			return NULL;
	}

	return AddGeometry(pcsFrame->m_acsLink + unKey, pstGeom);
}

/******************************************************************************
* Purpose :
* Desc : lRemIndex가 -1일 경우 모두 지운다.
* 150404 BOB
******************************************************************************/
BOOL AgcmGeometryDataAdmin::RemoveGeometry(AgcdGeometryData *pcsHead, INT32 lRemIndex)
{
	if (!pcsHead)
		return FALSE;

	AgcdGeometryData	*pcsPre		= NULL;
	AgcdGeometryData	*pcsCur		= pcsHead;
	INT32				lCount		= 0;
	while (pcsCur)
	{
		if ((lRemIndex == -1) || (lCount == lRemIndex))
		{
			if (pcsPre)
			{
				pcsPre->m_pcsNext	= pcsCur->m_pcsNext;
			}

			if (lRemIndex == -1)
			{
				pcsPre				= pcsCur->m_pcsNext;
			}

			delete pcsCur;

			if (lRemIndex -= -1)
			{
				pcsCur				= pcsPre;
				pcsPre				= NULL;
				continue;
			}
			else
			{
				return TRUE;
			}
		}

		pcsPre						= pcsCur;
		pcsCur						= pcsCur->m_pcsNext;
	}

	return (lRemIndex == -1) ? (TRUE) : (FALSE);
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
VOID AgcmGeometryDataAdmin::RemoveAllGeometry(AgcdGeometryDataLink *pcsLink)
{
	AgcdGeometryData	*pcsList	= pcsLink->m_pcsHead;
	AgcdGeometryData	*pcsTemp;
	while (pcsList)
	{
		pcsTemp						= pcsList->m_pcsNext;
		
		delete[] pcsList;

		pcsList						= pcsTemp;
	}

	pcsLink->m_pcsHead				= NULL;
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
BOOL AgcmGeometryDataAdmin::RemoveAllGeometry(AgcdGeometryFrame *pcsFrame)
{
	if (!pcsFrame->m_acsLink)
		return TRUE; // skip

	for (UINT8 unCount = 0; unCount < m_unNumAllocGeom; ++unCount)
	{
		RemoveAllGeometry(pcsFrame->m_acsLink + unCount);
	}

	delete[] pcsFrame->m_acsLink;
	pcsFrame->m_acsLink = NULL;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
VOID AgcmGeometryDataAdmin::SetNumAllocGeom(UINT8 unNum)
{
	m_unNumAllocGeom = unNum;
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
BOOL AgcmGeometryDataAdmin::CreateGeometryLink(AgcdGeometryFrame *pcsFrame)
{
	if (pcsFrame->m_acsLink)
		return FALSE;

	pcsFrame->m_acsLink		= new AgcdGeometryDataLink[m_unNumAllocGeom];
	if (!pcsFrame->m_acsLink)
		return FALSE;

	return TRUE;
}



/******************************************************************************
******************************************************************************/



/******************************************************************************
* Purpose :
*
* 140404 BOB
******************************************************************************/
AgcmGeometryDataManager::AgcmGeometryDataManager()
{
	SetModuleName("AgcmGeometryDataManager");

	InitializeVariable();
}

/******************************************************************************
* Purpose :
*
* 140404 BOB
******************************************************************************/
VOID AgcmGeometryDataManager::InitializeVariable()
{
	m_pcsAgcmCharacter				= NULL;
	m_pcsAgcmItem					= NULL;
	m_pcsAgcmObject					= NULL;
	m_unNumAttachedData				= 0;

	memset(m_paszKey, 0, sizeof(CHAR *) * AGCM_GDM_MAX_KEY_ARRAY);
}

/******************************************************************************
* Purpose :
*
* 140404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::OnAddModule()
{
	m_pcsAgcmCharacter				= (AgcmCharacter *)(GetModule("AgcmCharacter"));
	m_pcsAgcmItem					= (AgcmItem *)(GetModule("AgcmItem"));
	m_pcsAgcmObject					= (AgcmObject *)(GetModule("AgcmObject"));

	if ((!m_pcsAgcmCharacter) || (!m_pcsAgcmItem) || (!m_pcsAgcmObject))
		return FALSE;

	m_lAttachedDataIndexChar = m_pcsAgcmCharacter->GetAgpmCharacter()->AttachCharacterTemplateData(this, sizeof(AgcdGeometryFrame), NULL, GeometryFrameDestructCB);
	if (m_lAttachedDataIndexChar < 0)
		return FALSE;

	m_lAttachedDataIndexItem = m_pcsAgcmItem->GetAgpmItem()->AttachItemTemplateData(this, sizeof(AgcdGeometryFrame), NULL, GeometryFrameDestructCB);
	if (m_lAttachedDataIndexItem < 0)
		return FALSE;

	m_lAttachedDataIndexObjt = m_pcsAgcmObject->GetApmObjectModule()->AttachObjectTemplateData(this, sizeof(AgcdGeometryFrame), NULL, GeometryFrameDestructCB);
	if (m_lAttachedDataIndexObjt < 0)
		return FALSE;

	if (!AddStreamCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_OBJECT, StreamReadGeoemtryDataCB, StreamWriteGeoemtryDataCB, this))
		return FALSE;

	if (!AddStreamCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_CHARACTER, StreamReadGeoemtryDataCB, StreamWriteGeoemtryDataCB, this))
		return FALSE;

	if (!AddStreamCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_ITEM, StreamReadGeoemtryDataCB, StreamWriteGeoemtryDataCB, this))
		return FALSE;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::OnInit()
{
	m_csAdmin.SetNumAllocGeom(m_unNumAttachedData);

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::OnDestroy()
{
	INT32	lCount;

	for (lCount = 0; lCount < m_unNumAttachedData; ++lCount)
	{
		if (m_paszKey[lCount])
		{
			delete[] m_paszKey[lCount];
			m_paszKey[lCount] = NULL;
		}
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::ReleaseGeometryFrame(AgcdGeometryFrame *pcsFrame)
{
	return m_csAdmin.RemoveAllGeometry(pcsFrame);
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::GeometryFrameDestructCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData))
		return FALSE;

	AgcmGeometryDataManager		*pcsThis		= (AgcmGeometryDataManager *)(pClass);
	AgcdGeometryFrame			*pcsFrame		= pcsThis->GetGeometryFrame((ApBase *)(pData));
	if (!pcsFrame)
		return FALSE;

	return pcsThis->ReleaseGeometryFrame(pcsFrame);
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
AgcdGeometryFrame *AgcmGeometryDataManager::GetGeometryFrame(ApBase *pcsObj)
{
	switch (pcsObj->m_eType)
	{
	case APBASE_TYPE_OBJECT_TEMPLATE:
		{
			return GetGeometryFrame((ApdObjectTemplate *)(pcsObj));
		}
		break;

	case APBASE_TYPE_CHARACTER_TEMPLATE:
		{
			return GetGeometryFrame((AgpdCharacterTemplate *)(pcsObj));
		}
		break;

	case APBASE_TYPE_ITEM_TEMPLATE:
		{
			return GetGeometryFrame((AgpdItemTemplate *)(pcsObj));
		}
		break;
	}

	return NULL;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
AgcdGeometryFrame *AgcmGeometryDataManager::GetGeometryFrame(AgpdCharacterTemplate *pcsObj)
{
	return (AgcdGeometryFrame *)(m_pcsAgcmCharacter->GetAgpmCharacter()->GetAttachedModuleData(m_lAttachedDataIndexChar, (PVOID)(pcsObj)));
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
AgcdGeometryFrame *AgcmGeometryDataManager::GetGeometryFrame(AgpdItemTemplate *pcsObj)
{
	return (AgcdGeometryFrame *)(m_pcsAgcmItem->GetAgpmItem()->GetAttachedModuleData(m_lAttachedDataIndexItem, (PVOID)(pcsObj)));
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
AgcdGeometryFrame *AgcmGeometryDataManager::GetGeometryFrame(ApdObjectTemplate *pcsObj)
{
	return (AgcdGeometryFrame *)(m_pcsAgcmObject->GetApmObjectModule()->GetAttachedModuleData(m_lAttachedDataIndexObjt, (PVOID)(pcsObj)));
}

/******************************************************************************
* Purpose :
* Desc : OnInit 전에 호출해야하며, 실패시 -1을 return한다!
* 150404 BOB
******************************************************************************/
INT32 AgcmGeometryDataManager::AttachData(CHAR *szKey)
{
	if (m_unNumAttachedData >= AGCM_GDM_MAX_KEY_ARRAY)
		return -1;

	if (m_paszKey[m_unNumAttachedData])
		return -1;

	m_paszKey[m_unNumAttachedData] = new CHAR[AGCM_GDM_MAX_KEY_BUFFER];
	if (!m_paszKey[m_unNumAttachedData])
		return -1;

	strcpy(m_paszKey[m_unNumAttachedData], szKey);

	return m_unNumAttachedData++;
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
AcuGeometry *AgcmGeometryDataManager::AddGeometry(AgcdGeometryFrame *pcsFrame, CHAR *szKey, AcuGeometry *pstGeom)
{
	UINT8	unKeyIndex	= FindKeyIndex(szKey);
	if (unKeyIndex == m_unNumAttachedData)
		return FALSE;

	return AddGeometry(pcsFrame, unKeyIndex, pstGeom);
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
AcuGeometry *AgcmGeometryDataManager::AddGeometry(AgcdGeometryFrame *pcsFrame, UINT8 unKey, AcuGeometry *pstGeom)
{
	return m_csAdmin.AddGeometry(pcsFrame, unKey, pstGeom);
}

/******************************************************************************
* Purpose :
*
* 150604 BOB
******************************************************************************/
AcuGeometry	*AgcmGeometryDataManager::AddGeometry(AgcdGeometryDataLink *pcsLink, AcuGeometry *pstGeom)
{
	return m_csAdmin.AddGeometry(pcsLink, pstGeom);
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
AcuGeometry *AgcmGeometryDataManager::GetGeometry(AgcdGeometryFrame *pcsFrame, CHAR *szKey, INT32 lIndex, BOOL bCreate)
{
	UINT8	unKeyIndex	= FindKeyIndex(szKey);
	if (unKeyIndex == m_unNumAttachedData)
		return NULL;

	return GetGeometry(pcsFrame, unKeyIndex, lIndex, bCreate);
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
AcuGeometry *AgcmGeometryDataManager::GetGeometry(AgcdGeometryFrame *pcsFrame, UINT8 unKey, INT32 lIndex, BOOL bCreate)
{
	if (!pcsFrame->m_acsLink)
	{
		if (!m_csAdmin.CreateGeometryLink(pcsFrame))
			return NULL;
	}

	INT32				lCount		= 0;
	AgcdGeometryData	*pcsList	= (pcsFrame->m_acsLink + unKey)->m_pcsHead;
	while (pcsList)
	{
		if (lCount == lIndex)
			return &pcsList->m_stGeom;

		++lCount;
		pcsList						= pcsList->m_pcsNext;
	}

	if (bCreate)
		return AddGeometry(pcsFrame, unKey);

	return NULL;
}

/******************************************************************************
* Purpose :
*
* 150604 BOB
******************************************************************************/
AgcdGeometryDataLink *AgcmGeometryDataManager::GetGeometryDataLink(ApBase *pcsObj, CHAR *szKey)
{
	UINT8	unKeyIndex	= FindKeyIndex(szKey);
	if (unKeyIndex == m_unNumAttachedData)
		return NULL;

	return GetGeometryDataLink(pcsObj, unKeyIndex);
}

/******************************************************************************
* Purpose :
*
* 150604 BOB
******************************************************************************/
AgcdGeometryDataLink *AgcmGeometryDataManager::GetGeometryDataLink(ApBase *pcsObj, UINT8 unKey)
{
	AgcdGeometryFrame	*pcsFrame = GetGeometryFrame(pcsObj);
	if (!pcsFrame)
		return NULL;

	if (!pcsFrame->m_acsLink)
		m_csAdmin.CreateGeometryLink(pcsFrame);

	if (!pcsFrame->m_acsLink)
		return NULL;

	return pcsFrame->m_acsLink + unKey;
}

/******************************************************************************
* Purpose :
*
* 150604 BOB
******************************************************************************/
AgcdGeometryDataLink *AgcmGeometryDataManager::GetGeometryDataLink(AgpdCharacterTemplate *pcsObj, CHAR *szKey)
{
	UINT8	unKeyIndex	= FindKeyIndex(szKey);
	if (unKeyIndex == m_unNumAttachedData)
		return NULL;

	return GetGeometryDataLink(pcsObj, unKeyIndex);
}

/******************************************************************************
* Purpose :
*
* 150604 BOB
******************************************************************************/
AgcdGeometryDataLink *AgcmGeometryDataManager::GetGeometryDataLink(AgpdCharacterTemplate *pcsObj, UINT8 unKey)
{
	AgcdGeometryFrame	*pcsFrame = GetGeometryFrame(pcsObj);
	if (!pcsFrame)
		return NULL;

	if (!pcsFrame->m_acsLink)
		m_csAdmin.CreateGeometryLink(pcsFrame);

	if (!pcsFrame->m_acsLink)
		return NULL;

	return pcsFrame->m_acsLink + unKey;
}

/******************************************************************************
* Purpose :
*
* 150604 BOB
******************************************************************************/
AgcdGeometryDataLink *AgcmGeometryDataManager::GetGeometryDataLink(AgpdItemTemplate *pcsObj, CHAR *szKey)
{
	UINT8	unKeyIndex	= FindKeyIndex(szKey);
	if (unKeyIndex == m_unNumAttachedData)
		return NULL;

	return GetGeometryDataLink(pcsObj, unKeyIndex);
}

/******************************************************************************
* Purpose :
*
* 150604 BOB
******************************************************************************/
AgcdGeometryDataLink *AgcmGeometryDataManager::GetGeometryDataLink(AgpdItemTemplate *pcsObj, UINT8 unKey)
{
	AgcdGeometryFrame	*pcsFrame = GetGeometryFrame(pcsObj);
	if (!pcsFrame)
		return NULL;

	if (!pcsFrame->m_acsLink)
		m_csAdmin.CreateGeometryLink(pcsFrame);

	if (!pcsFrame->m_acsLink)
		return NULL;

	return pcsFrame->m_acsLink + unKey;
}

/******************************************************************************
* Purpose :
*
* 150604 BOB
******************************************************************************/
AgcdGeometryDataLink *AgcmGeometryDataManager::GetGeometryDataLink(ApdObjectTemplate *pcsObj, CHAR *szKey)
{
	UINT8	unKeyIndex	= FindKeyIndex(szKey);
	if (unKeyIndex == m_unNumAttachedData)
		return NULL;

	return GetGeometryDataLink(pcsObj, unKeyIndex);
}

/******************************************************************************
* Purpose :
*
* 150604 BOB
******************************************************************************/
AgcdGeometryDataLink *AgcmGeometryDataManager::GetGeometryDataLink(ApdObjectTemplate *pcsObj, UINT8 unKey)
{
	AgcdGeometryFrame	*pcsFrame = GetGeometryFrame(pcsObj);
	if (!pcsFrame)
		return NULL;

	if (!pcsFrame->m_acsLink)
		m_csAdmin.CreateGeometryLink(pcsFrame);

	if (!pcsFrame->m_acsLink)
		return NULL;

	return pcsFrame->m_acsLink + unKey;
}

/******************************************************************************
* Purpose :
*
* 150404 BOB
******************************************************************************/
UINT8 AgcmGeometryDataManager::FindKeyIndex(CHAR *szKey)
{
	UINT8 unIndex;

	for (unIndex = 0; unIndex < m_unNumAttachedData; ++unIndex)
	{
		if (!m_paszKey[unIndex])
			return m_unNumAttachedData; // 엥? -_-;;

		if (!strncmp(m_paszKey[unIndex], szKey, AGCM_GDM_MAX_KEY_BUFFER))
			break;
	}

	return unIndex;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::WriteObjGeomDataFile(CHAR *szPath)
{
	ApModuleStream			csStream;
	CHAR					szTemp[256];
	ApdObjectTemplate		*pcsTemplate;
	AgcdGeometryFrame		*pcsFrame;

	INT32 lIndex = 0;
	for (	pcsTemplate = m_pcsAgcmObject->GetApmObjectModule()->GetObjectTemplateSequence(&lIndex);
			pcsTemplate;
			pcsTemplate = m_pcsAgcmObject->GetApmObjectModule()->GetObjectTemplateSequence(&lIndex)		)
	{
		pcsFrame = GetGeometryFrame(pcsTemplate);
		if (!pcsFrame)
			return FALSE;

		sprintf(szTemp, "%d", pcsTemplate->m_lID);
		csStream.SetSection(szTemp);

		if (!csStream.EnumWriteCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_OBJECT, (PVOID)(pcsFrame), this))
			return FALSE;
	}

	csStream.Write(szPath);

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::WriteCharGeomDataFile(CHAR *szPath)
{
	ApModuleStream			csStream;
	CHAR					szTemp[256];
	AgpdCharacterTemplate	*pcsTemplate;
	AgcdGeometryFrame		*pcsFrame;

	INT32 lIndex = 0;
	for (	pcsTemplate = m_pcsAgcmCharacter->GetAgpmCharacter()->GetTemplateSequence(&lIndex);
			pcsTemplate;
			pcsTemplate = m_pcsAgcmCharacter->GetAgpmCharacter()->GetTemplateSequence(&lIndex)	)
	{
		pcsFrame = GetGeometryFrame(pcsTemplate);
		if (!pcsFrame)
			return FALSE;

		sprintf(szTemp, "%d", pcsTemplate->m_lID);
		csStream.SetSection(szTemp);

		if (!csStream.EnumWriteCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_CHARACTER, (PVOID)(pcsFrame), this))
			return FALSE;
	}

	csStream.Write(szPath);

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::WriteItemGeomDataFile(CHAR *szPath)
{
	ApModuleStream			csStream;
	CHAR					szTemp[256];
	AgpdItemTemplate		*pcsTemplate;
	AgcdGeometryFrame		*pcsFrame;

	for(AgpaItemTemplate::iterator it = m_pcsAgcmItem->GetAgpmItem()->csTemplateAdmin.begin(); it != m_pcsAgcmItem->GetAgpmItem()->csTemplateAdmin.end(); ++it)
	{
		pcsTemplate = it->second;
		pcsFrame = GetGeometryFrame(pcsTemplate);
		if (!pcsFrame)
			return FALSE;

		sprintf(szTemp, "%d", pcsTemplate->m_lID);
		csStream.SetSection(szTemp);

		if (!csStream.EnumWriteCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_ITEM, (PVOID)(pcsFrame), this))
			return FALSE;
	}

	csStream.Write(szPath);

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::StreamWriteGeoemtryDataCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if ((!pClass) || (!pData))
		return FALSE;

	AgcmGeometryDataManager		*pcsThis		= (AgcmGeometryDataManager *)(pClass);
	AgcdGeometryFrame			*pcsFrame		= (AgcdGeometryFrame *)(pData);

	if (!pcsFrame->m_acsLink)
		return TRUE;			// skip

	CHAR						szName[256];
	CHAR						szValue[256];
	CHAR						szTemp[256];
	INT32						lDataCount;
	AcuGeometry					*pstGeom;

	for (UINT8 unCount = 0; unCount < pcsThis->GetAttachedDataNum(); ++unCount)
	{
		lDataCount								= 0;

		AgcdGeometryData		*pstList		= (pcsFrame->m_acsLink + unCount)->m_pcsHead;
		while (pstList)
		{
			pstGeom								= &(pstList->m_stGeom);

			switch (pstGeom->type)
			{
			case AUBLOCKING_TYPE_BOX:
//			case AUBLOCKING_TYPE_MINBOX:
				{
					print_compact_format(szTemp,
						"%f:%f:%f:%f:%f:%f",
						pstGeom->data.box.inf.x,
						pstGeom->data.box.inf.y,
						pstGeom->data.box.inf.z,
						pstGeom->data.box.sup.x,
						pstGeom->data.box.sup.y,
						pstGeom->data.box.sup.z		);
				}
				break;

			case AUBLOCKING_TYPE_SPHERE:
				{
					print_compact_format(szTemp,
						"%f:%f:%f:%f",
						pstGeom->data.sphere.center.x,
						pstGeom->data.sphere.center.y,
						pstGeom->data.sphere.center.z,
						pstGeom->data.sphere.radius		);
				}
				break;

			case AUBLOCKING_TYPE_CYLINDER:
				{
					print_compact_format(szTemp,
						"%f:%f:%f:%f:%f",
						pstGeom->data.cylinder.center.x,
						pstGeom->data.cylinder.center.y,
						pstGeom->data.cylinder.center.z,
						pstGeom->data.cylinder.height,
						pstGeom->data.cylinder.radius		);
				}
				break;

			default:
				return FALSE;
			}

/*			INT32	lType = pstGeom->type;
			if (lType == AUBLOCKING_TYPE_MINBOX)
				lType = AUBLOCKING_TYPE_BOX;

			sprintf(szValue, "%s:%d:", pcsThis->GetAttachedDataName(unCount), lType);*/
			sprintf(szValue, "%s:%d:", pcsThis->GetAttachedDataName(unCount), pstGeom->type);
			strcat(szValue, szTemp);

			sprintf(szName, "%s%d%d", AGCM_GDM_INI_NAME_DATA, unCount, lDataCount);

			pStream->WriteValue(szName, szValue);

			++lDataCount;
			pstList								= pstList->m_pcsNext;
		}
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::ReadObjGeomDataFile(CHAR *szPath)
{
	ApModuleStream			csStream;
	ApdObjectTemplate		*pcsApdObjectTemplate;
	AgcdGeometryFrame		*pcsFrame;

	if (!csStream.Open(szPath))
		return FALSE;

	INT16	nNumKeys = csStream.GetNumSections();
	for (INT16 i = 0; i < nNumKeys; ++i)
	{
		pcsApdObjectTemplate = m_pcsAgcmObject->GetApmObjectModule()->GetObjectTemplate(atoi(csStream.ReadSectionName(i)));
		if (!pcsApdObjectTemplate)
			return FALSE;

		pcsFrame = GetGeometryFrame(pcsApdObjectTemplate);
		if (!pcsFrame)
			return FALSE;

		if (!csStream.EnumReadCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_OBJECT, (PVOID)(pcsFrame), this))
			return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::ReadCharGeomDataFile(CHAR *szPath)
{
	ApModuleStream			csStream;
	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate;
	AgcdGeometryFrame		*pcsFrame;

	if (!csStream.Open(szPath))
		return FALSE;

	INT16	nNumKeys = csStream.GetNumSections();
	for (INT16 i = 0; i < nNumKeys; ++i)
	{
		pcsAgpdCharacterTemplate = m_pcsAgcmCharacter->GetAgpmCharacter()->GetCharacterTemplate(atoi(csStream.ReadSectionName(i)));
		if (!pcsAgpdCharacterTemplate)
			return FALSE;

		pcsFrame = GetGeometryFrame(pcsAgpdCharacterTemplate);
		if (!pcsFrame)
			return FALSE;

		if (!csStream.EnumReadCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_CHARACTER, (PVOID)(pcsFrame), this))
			return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::ReadItemGeomDataFile(CHAR *szPath)
{
	ApModuleStream			csStream;
	AgpdItemTemplate		*pcsAgpdItemTemplate;
	AgcdGeometryFrame		*pcsFrame;

	if (!csStream.Open(szPath))
		return FALSE;

	INT16	nNumKeys = csStream.GetNumSections();
	for (INT16 i = 0; i < nNumKeys; ++i)
	{
		pcsAgpdItemTemplate = m_pcsAgcmItem->GetAgpmItem()->GetItemTemplate(atoi(csStream.ReadSectionName(i)));
		if (!pcsAgpdItemTemplate)
			return FALSE;

		pcsFrame = GetGeometryFrame(pcsAgpdItemTemplate);
		if (!pcsFrame)
			return FALSE;

		if (!csStream.EnumReadCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_ITEM, (PVOID)(pcsFrame), this))
			return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::SetGeomData(CHAR *szBuffer, AgcdGeometryFrame *pcsFrame)
{
	CHAR	szKey[256];
	INT32	lLen				= strlen(szBuffer);
	INT32	lProgress			= 0;

	for(;; ++lProgress)
	{
		if (lProgress >= lLen)
			return FALSE;

		if (szBuffer[lProgress] == ':')
		{
			szKey[lProgress]	= '\0';
			++lProgress;
			break;
		}

		szKey[lProgress]		= szBuffer[lProgress];
	}



	AcuGeometry	stGeom;

	switch (atoi(szBuffer + lProgress))
	{
	case AUBLOCKING_TYPE_BOX:
		{
			sscanf(szBuffer + lProgress,
				"%d:%f:%f:%f:%f:%f:%f",
				&stGeom.type,
				&stGeom.data.box.inf.x,
				&stGeom.data.box.inf.y,
				&stGeom.data.box.inf.z,
				&stGeom.data.box.sup.x,
				&stGeom.data.box.sup.y,
				&stGeom.data.box.sup.z		);
		}
		break;

	case AUBLOCKING_TYPE_SPHERE:
		{
			sscanf(szBuffer + lProgress,
				"%d:%f:%f:%f:%f",
				&stGeom.type,
				&stGeom.data.sphere.center.x,
				&stGeom.data.sphere.center.y,
				&stGeom.data.sphere.center.z,
				&stGeom.data.sphere.radius		);
		}
		break;

	case AUBLOCKING_TYPE_CYLINDER:
		{
			sscanf(szBuffer + lProgress,
				"%d:%f:%f:%f:%f:%f",
				&stGeom.type,
				&stGeom.data.cylinder.center.x,
				&stGeom.data.cylinder.center.y,
				&stGeom.data.cylinder.center.z,
				&stGeom.data.cylinder.height,
				&stGeom.data.cylinder.radius	);
		}
		break;

	default:
		return FALSE;
	}

	if (!AddGeometry(pcsFrame, szKey, &stGeom))
		return FALSE;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::StreamReadGeoemtryDataCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if ((!pClass) || (!pData))
		return FALSE;

	AgcmGeometryDataManager		*pcsThis		= (AgcmGeometryDataManager *)(pClass);
	AgcdGeometryFrame			*pcsFrame		= (AgcdGeometryFrame *)(pData);

	const CHAR					*szValueName;
	CHAR						szValue[256];

	while (pStream->ReadNextValue())
	{
		szValueName								= pStream->GetValueName();

		if (!strncmp(szValueName, AGCM_GDM_INI_NAME_DATA, strlen(AGCM_GDM_INI_NAME_DATA)))
		{
			pStream->GetValue(szValue, 256);
			if (!pcsThis->SetGeomData(szValue, pcsFrame))
				return FALSE;
		}
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
UINT8 AgcmGeometryDataManager::GetAttachedDataNum()
{
	return m_unNumAttachedData;
}

/******************************************************************************
* Purpose :
*
* 260404 BOB
******************************************************************************/
CHAR *AgcmGeometryDataManager::GetAttachedDataName(UINT8 unIndex)
{
	return m_paszKey[unIndex];
}

/******************************************************************************
* Purpose :
*
* 150604 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::ResetGeometryList(ApBase *pcsObj, CHAR *szKey)
{
	AgcdGeometryFrame	*pcsFrame	= GetGeometryFrame(pcsObj);
	if (!pcsFrame)
		return FALSE;

	return ResetGeometryList(pcsFrame, szKey);
}

/******************************************************************************
* Purpose :
*
* 150604 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::ResetGeometryList(AgcdGeometryFrame *pcsFrame, CHAR *szKey)
{
	if (!pcsFrame->m_acsLink)
		return TRUE; // skip

	UINT8				unKey		= FindKeyIndex(szKey);
	if (unKey == m_unNumAttachedData)
		return FALSE;

	return ResetGeometryList(&((pcsFrame->m_acsLink)->m_pcsHead));
}

/******************************************************************************
* Purpose :
*
* 150604 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::ResetGeometryList(AgcdGeometryData **ppcsHead)
{
	if (!m_csAdmin.RemoveGeometry(*(ppcsHead)))
		return FALSE;

	*(ppcsHead)						= NULL;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 061004 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::CopyGeomFrame(AgcdGeometryFrame *pcsDestFrame, AgcdGeometryFrame *pcsSrcFrame)
{
	if (!pcsSrcFrame->m_acsLink)
		return FALSE;

	AgcdGeometryData	*pcsList;

	for (INT32 lCount = 0; lCount < m_unNumAttachedData; ++lCount)
	{
		pcsList = (pcsSrcFrame->m_acsLink + lCount)->m_pcsHead;
		while (pcsList)
		{
			m_csAdmin.AddGeometry(pcsDestFrame, lCount, &pcsList->m_stGeom);

			pcsList	= pcsList->m_pcsNext;
		}
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 061004 BOB
******************************************************************************/
INT32 AgcmGeometryDataManager::FindSectionIndex(INT32 lFindSection, INT32 *alSections, INT32 lNumSections, INT32 lStart)
{
	INT32 lIndex;

	for (lIndex = lStart; lIndex < lNumSections; ++lIndex)
	{
		if (alSections[lIndex] == lFindSection)
			return lIndex;

		if (alSections[lIndex] == -1)
			break;
	}

	if (lStart != 0)
	{
		for (lIndex = 0; lIndex < lStart; ++lIndex)
		{
			if (alSections[lIndex] == lFindSection)
				return lIndex;
		}
	}

	return -1;
}

/******************************************************************************
* Purpose :
*
* 061004 BOB
******************************************************************************/
BOOL AgcmGeometryDataManager::MergeGeomDataFile(CHAR *szDestPath, CHAR *szSrcPath1, CHAR *szSrcPath2)
{
	ApModuleStream	csStream1, csStream2;
	INT32			lNumSections;

	{
		if (!csStream1.Open(szSrcPath1))
			return FALSE; // skip
	}

	{
		if (!csStream2.Open(szSrcPath2))
			return FALSE; // skip
	}

	lNumSections		= csStream1.GetNumSections();
	if (csStream2.GetNumSections() > lNumSections)
		lNumSections	= csStream2.GetNumSections();


	// 할당해보세~~~
	AgcdGeometryFrame	*pcsDestFrame		= new AgcdGeometryFrame[lNumSections];
	AgcdGeometryFrame	*pcsFrame1			= new AgcdGeometryFrame[lNumSections];
	AgcdGeometryFrame	*pcsFrame2			= new AgcdGeometryFrame[lNumSections];

	INT32				*plSectionIndex		= new INT32[lNumSections];
	INT32				*plSectionIndex1	= new INT32[lNumSections];
	INT32				*plSectionIndex2	= new INT32[lNumSections];

	memset(plSectionIndex, -1, sizeof(INT32) * lNumSections);
	memset(plSectionIndex1, -1, sizeof(INT32) * lNumSections);
	memset(plSectionIndex2, -1, sizeof(INT32) * lNumSections);

	CHAR				szValue[256];
	const CHAR			*pszSection;
	INT32				lSections;

	// 읽어보세~~~
	for (lSections = 0; lSections < lNumSections; ++lSections)
	{
		pszSection = csStream1.ReadSectionName(lSections);
		if (pszSection)
		{
			plSectionIndex1[lSections] = atoi(pszSection);

			csStream1.SetValueID(0);

			while (csStream1.ReadNextValue())
			{
				if (!strncmp(csStream1.GetValueName(), AGCM_GDM_INI_NAME_DATA, strlen(AGCM_GDM_INI_NAME_DATA)))
				{
					csStream1.GetValue(szValue, 256);
					SetGeomData(szValue, pcsFrame1 + lSections);
				}
			}
		}

		pszSection = csStream2.ReadSectionName(lSections);
		if (pszSection)
		{
			plSectionIndex2[lSections] = atoi(pszSection);

			csStream2.SetValueID(0);

			while (csStream2.ReadNextValue())
			{
				if (!strncmp(csStream2.GetValueName(), AGCM_GDM_INI_NAME_DATA, strlen(AGCM_GDM_INI_NAME_DATA)))
				{
					csStream2.GetValue(szValue, 256);
					SetGeomData(szValue, pcsFrame2 + lSections);
				}
			}
		}
	}

	INT32	lFindSectionIndex;
	for (lSections = 0; lSections < lNumSections; ++lSections)
	{
		plSectionIndex[lSections] = plSectionIndex1[lSections];

		if (pcsFrame1[lSections].m_acsLink)
		{
			CopyGeomFrame(pcsDestFrame + lSections, pcsFrame1 + lSections);
		}
		else
		{
			lFindSectionIndex = FindSectionIndex(plSectionIndex1[lSections], plSectionIndex2, lNumSections, lSections);
			if (lFindSectionIndex == -1)
				continue;
			if (!pcsFrame2[lFindSectionIndex].m_acsLink)
				continue;
			
			CopyGeomFrame(pcsDestFrame + lSections, pcsFrame2 + lFindSectionIndex);
		}
	}

/*		switch (eMergeFlags)
		{
		case E_AGCM_GEOM_DATA_MERGE_FLAGS_ADD:
			{
				if (pcsFrame1[lSections].m_acsLink)
					CopyGeomFrame(pcsDestFrame + lSections, pcsFrame1 + lSections);
				if (pcsFrame2[lSections].m_acsLink)
					CopyGeomFrame(pcsDestFrame + lSections, pcsFrame2 + lSections);
			}
			break;

		case E_AGCM_GEOM_DATA_MERGE_FLAGS_SRC1:
			{
				if (pcsFrame1[lSections].m_acsLink)
					CopyGeomFrame(pcsDestFrame + lSections, pcsFrame1 + lSections);
				else if (pcsFrame2[lSections].m_acsLink)
					CopyGeomFrame(pcsDestFrame + lSections, pcsFrame2 + lSections);
			}
			break;

		case E_AGCM_GEOM_DATA_MERGE_FLAGS_SRC2:
			{
				if (pcsFrame2[lSections].m_acsLink)
					CopyGeomFrame(pcsDestFrame + lSections, pcsFrame2 + lSections);
				else if (pcsFrame1[lSections].m_acsLink)
					CopyGeomFrame(pcsDestFrame + lSections, pcsFrame1 + lSections);
			}
			break;

		case E_AGCM_GEOM_DATA_MERGE_FLAGS_SKIP:
			{
				if ((!pcsFrame1[lSections].m_acsLink) || (!pcsFrame2[lSections].m_acsLink))
				{
					if (pcsFrame1[lSections].m_acsLink)
						CopyGeomFrame(pcsDestFrame + lSections, pcsFrame1 + lSections);
					if (pcsFrame2[lSections].m_acsLink)
						CopyGeomFrame(pcsDestFrame + lSections, pcsFrame2 + lSections);
				}
			}
			break;
		}
	}*/

	// 써보세~~~
	{
		AcuGeometry			*pstGeom;
		AgcdGeometryData	*pstList;
		INT32				lDataCount;
		CHAR				szSectionName[256], szTemp[256], szName[256], szValue[256];
		ApModuleStream		csWriteStream;
		csWriteStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

		for (lSections = 0; lSections < lNumSections; ++lSections)
		{
			sprintf(szSectionName, "%d", plSectionIndex[lSections]);
			csWriteStream.SetSection(szSectionName);

			csWriteStream.WriteValue("ModuleData1", "AgcmGeometryDataManager");

			if (!pcsDestFrame[lSections].m_acsLink)
			{
				csWriteStream.WriteValue("EnumEnd", "0");
				continue;
			}

			for (UINT8 unCount = 0; unCount < m_unNumAttachedData; ++unCount)
			{
				lDataCount		= 0;

				pstList			= (pcsDestFrame[lSections].m_acsLink + unCount)->m_pcsHead;
				while (pstList)
				{
					pstGeom		= &(pstList->m_stGeom);

					switch (pstGeom->type)
					{
					case AUBLOCKING_TYPE_BOX:
						{
							print_compact_format(szTemp,
								"%f:%f:%f:%f:%f:%f",
								pstGeom->data.box.inf.x,
								pstGeom->data.box.inf.y,
								pstGeom->data.box.inf.z,
								pstGeom->data.box.sup.x,
								pstGeom->data.box.sup.y,
								pstGeom->data.box.sup.z		);
						}
						break;

					case AUBLOCKING_TYPE_SPHERE:
						{
							print_compact_format(szTemp,
								"%f:%f:%f:%f",
								pstGeom->data.sphere.center.x,
								pstGeom->data.sphere.center.y,
								pstGeom->data.sphere.center.z,
								pstGeom->data.sphere.radius		);
						}
						break;

					case AUBLOCKING_TYPE_CYLINDER:
						{
							print_compact_format(szTemp,
								"%f:%f:%f:%f:%f",
								pstGeom->data.cylinder.center.x,
								pstGeom->data.cylinder.center.y,
								pstGeom->data.cylinder.center.z,
								pstGeom->data.cylinder.height,
								pstGeom->data.cylinder.radius		);
						}
						break;

					default:
						{
							return FALSE;
						}
					}

					sprintf(szValue, "%s:%d:", GetAttachedDataName(unCount), pstGeom->type);
					strcat(szValue, szTemp);

					sprintf(szName, "%s%d%d", AGCM_GDM_INI_NAME_DATA, unCount, lDataCount);

					csWriteStream.WriteValue(szName, szValue);

					++lDataCount;

					pstList								= pstList->m_pcsNext;
				}
			}

			csWriteStream.WriteValue("EnumEnd", "0");
		}

		csWriteStream.Write(szDestPath);
	}


	// 날려보세~~~
	for (lSections = 0; lSections < lNumSections; ++lSections)
	{
		ReleaseGeometryFrame(pcsFrame1 + lSections);
		ReleaseGeometryFrame(pcsFrame2 + lSections);
		ReleaseGeometryFrame(pcsDestFrame + lSections);
	}

	delete[] pcsFrame1;
	delete[] pcsFrame2;
	delete[] pcsDestFrame;

	delete[] plSectionIndex;
	delete[] plSectionIndex1;
	delete[] plSectionIndex2;

	return TRUE;
}

/******************************************************************************
******************************************************************************/
