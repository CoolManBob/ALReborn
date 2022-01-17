#include "AgpmEventTeleport.h"
#include "ApModuleStream.h"

BOOL AgpmEventTeleport::StreamReadGroup(CHAR *szFile, BOOL bDecryption)
{
	ApModuleStream		csStream;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(szFile, 0, bDecryption);

	UINT16				nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for (int i = 0; i < nNumKeys; ++i)
	{
		// Section Name은 TID 이다.
		const CHAR	*szGroupName = csStream.ReadSectionName(i);
		if (!strlen(szGroupName))
		{
			OutputDebugString("AgpmEventTeleport::StreamReadGroup() Error (0) !!!\n");
			return FALSE;
		}

		// add teleport group
		AgpdTeleportGroup *pcsGroup	= AddTeleportGroup((CHAR *) szGroupName);
		if (!pcsGroup)
		{
			OutputDebugString("AgpmEventTeleport::StreamReadGroup() Error (1) !!!\n");
			continue;
			//return FALSE;
		}

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMEVENT_TELEPORT_DATA_TYPE_GROUP, pcsGroup, this))
		{
			OutputDebugString("AgpmEventTeleport::StreamReadGroup() Error (2) !!!\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmEventTeleport::StreamWriteGroup(CHAR *szFile, BOOL bEncryption)
{
	ApModuleStream			csStream;
	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	INT32					lIndex = 0;
	AgpdTeleportGroup		**ppcsTeleportGroup	= (AgpdTeleportGroup **) m_csAdminGroup.GetObjectSequence(&lIndex);
	while (ppcsTeleportGroup && *ppcsTeleportGroup)
	{
		// GroupName으로 Section을 설정하고
		csStream.SetSection((*ppcsTeleportGroup)->m_szGroupName);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGPMEVENT_TELEPORT_DATA_TYPE_GROUP, *ppcsTeleportGroup, this))
		{
			OutputDebugString("AgpmEventTeleport::StreamWriteGroup() Error (1) !!!\n");
			return FALSE;
		}

		ppcsTeleportGroup = (AgpdTeleportGroup **) m_csAdminGroup.GetObjectSequence(&lIndex);
	}

	return csStream.Write(szFile, 0, bEncryption);
}

BOOL AgpmEventTeleport::StreamReadPoint(CHAR *szFile, BOOL bDecryption)
{
	ApModuleStream		csStream;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(szFile, 0, bDecryption);

	UINT16				nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for (int i = 0; i < nNumKeys; ++i)
	{
		const CHAR	*szPointName = csStream.ReadSectionName(i);
		if (!strlen(szPointName))
		{
			OutputDebugString("AgpmEventTeleport::StreamReadPoint() Error (0) !!!\n");
			return FALSE;
		}

		// add teleport group
		AgpdTeleportPoint *pcsPoint	= AddTeleportPoint((CHAR *) szPointName);
		if (!pcsPoint)
		{
			OutputDebugString("AgpmEventTeleport::StreamReadPoint() Error (1) !!!\n");
			continue;
			//return FALSE;
		}

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMEVENT_TELEPORT_DATA_TYPE_POINT, pcsPoint, this))
		{
			OutputDebugString("AgpmEventTeleport::StreamReadPoint() Error (2) !!!\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmEventTeleport::StreamWritePoint(CHAR *szFile, BOOL bEncryption)
{
	ApModuleStream			csStream;
	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	INT32					lIndex = 0;
	AgpdTeleportPoint		**ppcsTeleportPoint	= (AgpdTeleportPoint **) m_csAdminPoint.GetObjectSequence(&lIndex);
	while (ppcsTeleportPoint && *ppcsTeleportPoint)
	{
		// GroupName으로 Section을 설정하고
		csStream.SetSection((*ppcsTeleportPoint)->m_szPointName);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGPMEVENT_TELEPORT_DATA_TYPE_POINT, *ppcsTeleportPoint, this))
		{
			OutputDebugString("AgpmEventTeleport::StreamWritePoint() Error (1) !!!\n");
			return FALSE;
		}

		ppcsTeleportPoint	= (AgpdTeleportPoint **) m_csAdminPoint.GetObjectSequence(&lIndex);
	}

	return csStream.Write(szFile, 0, bEncryption);
}

BOOL AgpmEventTeleport::GroupWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgpmEventTeleport	*pThis			= (AgpmEventTeleport *)	pClass;
	AgpdTeleportGroup	*pcsGroup		= (AgpdTeleportGroup *)	pData;

	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_DESCRIPTION, pcsGroup->m_szDescription))
	{
		OutputDebugString("AgpmEventTeleport::GroupWriteCB() Error (1) !!!\n");
		return FALSE;
	}

	return TRUE;
}

BOOL AgpmEventTeleport::GroupReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgpmEventTeleport	*pThis			= (AgpmEventTeleport *)	pClass;
	AgpdTeleportGroup	*pcsGroup		= (AgpdTeleportGroup *)	pData;

	const CHAR			*szValueName	= NULL;

	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_DESCRIPTION))
		{
			pStream->GetValue(pcsGroup->m_szDescription, AGPD_TELEPORT_MAX_GROUP_DESCRIPTION);
		}
	}

	return TRUE;
}

BOOL AgpmEventTeleport::PointWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgpmEventTeleport	*pThis			= (AgpmEventTeleport *)	pClass;
	AgpdTeleportPoint	*pcsPoint		= (AgpdTeleportPoint *)	pData;
	AgpdTeleportGroup	*pcsGroup;
	INT_PTR				lIndex;

	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_DESCRIPTION, pcsPoint->m_szDescription))
	{
		OutputDebugString("AgpmEventTeleport::PointWriteCB() Error (1) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_RADIUS_MIN, pcsPoint->m_fRadiusMin))
	{
		OutputDebugString("AgpmEventTeleport::PointWriteCB() Error (2) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_RADIUS_MAX, pcsPoint->m_fRadiusMax))
	{
		OutputDebugString("AgpmEventTeleport::PointWriteCB() Error (0) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_POINT_TYPE, pcsPoint->m_eType))
	{
		OutputDebugString("AgpmEventTeleport::PointWriteCB() Error (3) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_POINT_REGION_TYPE, pcsPoint->m_eRegionType))
	{
		OutputDebugString("AgpmEventTeleport::PointWriteCB() Error (12) !!!\n");
		return FALSE;
	}
	
	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_POINT_SPECIAL_TYPE, pcsPoint->m_eSpecialType))
	{
		OutputDebugString("AgpmEventTeleport::PointWriteCB() Error (13) !!!\n");
		return FALSE;
	}

	CHAR	szBuffer[64];
	ZeroMemory(szBuffer, sizeof(CHAR) * 64);

	INT32	lStrLen	= print_compact_format(szBuffer, "%f:%f:%f", pcsPoint->m_uniTarget.m_stPos.x, pcsPoint->m_uniTarget.m_stPos.y, pcsPoint->m_uniTarget.m_stPos.z);
	ASSERT(lStrLen < 64);

	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_POINT_POS, szBuffer))
	{
		OutputDebugString("AgpmEventTeleport::PointWriteCB() Error (4) !!!\n");
		return FALSE;
	}

	ZeroMemory(szBuffer, sizeof(CHAR) * 64);

	INT32	lStrlen	= sprintf(szBuffer, "%d:%d", pcsPoint->m_uniTarget.m_stBase.m_eBaseType, pcsPoint->m_uniTarget.m_stBase.m_lID);
	ASSERT(lStrLen < 64);

	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_POINT_BASE, szBuffer))
	{
		OutputDebugString("AgpmEventTeleport::PointWriteCB() Error (5) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_POINT_EVENT, (INT32) pcsPoint->m_bAttachEvent))
		return FALSE;

	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_USE_TYPE, (INT32) pcsPoint->m_byUseType))
		return FALSE;

	lIndex = 0;
	for (pcsGroup = pThis->GetSequenceGroupInPoint(pcsPoint, &lIndex); pcsGroup; pcsGroup = pThis->GetSequenceGroupInPoint(pcsPoint, &lIndex))
	{
		if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_POINT_GROUP_NAME, pcsGroup->m_szGroupName))
			return FALSE;
	}

	lIndex = 0;
	for (pcsGroup = pThis->GetSequenceTargetGroup(pcsPoint, &lIndex); pcsGroup; pcsGroup = pThis->GetSequenceTargetGroup(pcsPoint, &lIndex))
	{
		if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_POINT_TARGET_NAME, pcsGroup->m_szGroupName))
			return FALSE;
	}

	return TRUE;
}

BOOL AgpmEventTeleport::PointReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgpmEventTeleport	*pThis			= (AgpmEventTeleport *)	pClass;
	AgpdTeleportPoint	*pcsPoint		= (AgpdTeleportPoint *)	pData;
	AgpdTeleportGroup	*pcsGroup;

	const CHAR			*szValueName	= NULL;

	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_DESCRIPTION))
		{
			pStream->GetValue(pcsPoint->m_szDescription, AGPD_TELEPORT_MAX_POINT_DESCRIPTION);
		}
		else if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_RADIUS_MIN))
		{
			pStream->GetValue(&pcsPoint->m_fRadiusMin);
		}
		else if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_RADIUS_MAX))
		{
			pStream->GetValue(&pcsPoint->m_fRadiusMax);
		}
		else if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_POINT_TYPE))
		{
			INT32	lPointType	= 0;
			pStream->GetValue(&lPointType);

			pcsPoint->m_eType	= (AgpdTeleportTarget) lPointType;
		}
		else if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_POINT_REGION_TYPE))
		{
			INT32	lPointRegionType	= 0;
			pStream->GetValue(&lPointRegionType);

			pcsPoint->m_eRegionType	= (AgpdTeleportRegionType) lPointRegionType;
		}
		else if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_POINT_SPECIAL_TYPE))
		{
			INT32	lPointSpecialType	= 0;
			pStream->GetValue(&lPointSpecialType);

			pcsPoint->m_eSpecialType	= (AgpdTeleportSpecialType) lPointSpecialType;
		}
		else if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_POINT_POS))
		{
			if (pcsPoint->m_eType == AGPDTELEPORT_TARGET_TYPE_POS)
			{
				CHAR	szBuffer[64];
				ZeroMemory(szBuffer, sizeof(CHAR) * 64);

				pStream->GetValue(szBuffer, 64);

				sscanf(szBuffer, "%f:%f:%f", &pcsPoint->m_uniTarget.m_stPos.x, &pcsPoint->m_uniTarget.m_stPos.y, &pcsPoint->m_uniTarget.m_stPos.z);
			}
		}
		else if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_POINT_BASE))
		{
			if (pcsPoint->m_eType == AGPDTELEPORT_TARGET_TYPE_BASE)
			{
				CHAR	szBuffer[64];
				ZeroMemory(szBuffer, sizeof(CHAR) * 64);

				pStream->GetValue(szBuffer, 64);

				int		nBaseType	= 0;
				int		nBaseID		= 0;

				sscanf(szBuffer, "%d:%d", &nBaseType, &nBaseID);

				pcsPoint->m_uniTarget.m_stBase.m_eBaseType	= (ApBaseType)	nBaseType;
				pcsPoint->m_uniTarget.m_stBase.m_lID		= (INT32)		nBaseID;
			}
		}
		else if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_POINT_GROUP_NAME))
		{
			pcsGroup = pThis->GetTeleportGroup((CHAR *) pStream->GetValue());
			if (!pcsGroup)
			{
				MD_SetErrorMessage("Can't Get Group !!!");
			}
			else if (!pThis->AddPointToGroup(pcsGroup, pcsPoint))
			{
				MD_SetErrorMessage("Can't Add Group To Point !!!");
			}
		}
		else if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_POINT_TARGET_NAME))
		{
			pcsGroup = pThis->GetTeleportGroup((CHAR *) pStream->GetValue());
			if (!pcsGroup)
			{
				MD_SetErrorMessage("Can't Get Group !!!");
			}
			else if (!pThis->AddTargetGroupToPoint(pcsGroup, pcsPoint))
			{
				MD_SetErrorMessage("Can't Add Target Group To Point !!!");
			}
		}
		else if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_POINT_EVENT))
		{
			if (!pStream->GetValue((INT32 *) &pcsPoint->m_bAttachEvent))
				return FALSE;
		}
		else if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_USE_TYPE))
		{
			if (!pStream->GetValue((INT32 *) &pcsPoint->m_byUseType))
				return FALSE;
		}
	}

	return TRUE;
}

BOOL	AgpmEventTeleport::CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ApdEvent *				pstEvent = (ApdEvent *) pData;
	AgpmEventTeleport *		pThis = (AgpmEventTeleport *) pClass;
	AgpdTeleportAttach *	pstAttachData = (AgpdTeleportAttach *) pstEvent->m_pvData;

	ApModuleStream *		pStream			= (ApModuleStream *) pCustData;

	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_POINT_NAME, pstAttachData->m_pcsPoint ? pstAttachData->m_pcsPoint->m_szPointName : ""))
	{
		OutputDebugString("AgpmEventTeleport::CBStreamWriteEvent() Error (1) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMTELEPORT_STREAM_NAME_EVENT_END, 0))
		return FALSE;

	return TRUE;
}

BOOL	AgpmEventTeleport::CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ApdEvent *				pstEvent = (ApdEvent *) pData;
	AgpmEventTeleport *		pThis = (AgpmEventTeleport *) pClass;
	AgpdTeleportAttach *	pstAttachData = (AgpdTeleportAttach *) pstEvent->m_pvData;
	AgpdTeleportPoint *		pcsPoint;

	ApModuleStream *		pStream			= (ApModuleStream *) pCustData;

	const CHAR				*szValueName	= NULL;

	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_POINT_NAME))
		{
			pcsPoint = pThis->GetTeleportPoint((CHAR *) pStream->GetValue());
			if (!pcsPoint)
			{
				TRACE( "텔레포트 포인트 '%s' 를 찾을수 없습니다.\n" , (CHAR *) pStream->GetValue() ); 
				//ASSERT(!"Can't Get Teleport Point !!!");
			}
			else
			{
				pstAttachData->m_lPointID = pcsPoint->m_lID;
				pstAttachData->m_pcsPoint = pcsPoint;

				pcsPoint->m_uniTarget.m_stBase.m_eBaseType = pstEvent->m_pcsSource->m_eType;
				pcsPoint->m_uniTarget.m_stBase.m_lID = pstEvent->m_pcsSource->m_lID;

				pcsPoint->m_pstAttach = pstAttachData;
			}
		}
		else if (!strcmp(szValueName, AGPMTELEPORT_STREAM_NAME_EVENT_END))
			break;
	}

	return TRUE;
}

BOOL	AgpmEventTeleport::StreamReadFee(CHAR *szFile, BOOL bDecryption)
{
	INT32					lRowCount;
	INT32					lRow;
	AuExcelTxtLib			csStream;
	CHAR *					szSource;
	CHAR *					szTarget;
	INT32					lFee;

	if(!csStream.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgpmEventTeleport::StreamReadFee() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}

	lRowCount	= csStream.GetRow() - 1;

	m_csAdminFee.InitializeObject(sizeof(INT32), lRowCount);
	m_pszFeeName = new CHAR *[lRowCount + 1];		//	메모리 overrun 나길래 + 1 해줌 2005.08.19. By SungHoon
	memset(m_pszFeeName, 0, sizeof(CHAR *) * lRowCount);

	for(lRow = 1; lRow <= lRowCount; ++lRow)
	{
		szSource	= csStream.GetData(0, lRow);
		szTarget	= csStream.GetData(1, lRow);
		lFee		= csStream.GetDataToInt(2, lRow);

		if (!szSource || !szTarget || !lFee)
			continue;

		m_pszFeeName[lRow] = new CHAR [strlen(szSource) + strlen(szTarget) + 2];
		sprintf(m_pszFeeName[lRow], "%s-%s", szSource, szTarget);

		if (!m_csAdminFee.AddObject((PVOID) &lFee, m_pszFeeName[lRow]))
		{
			OutputDebugString("AgpmEventTeleport::StreamReadFee() Error (2) !!!\n");
			csStream.CloseFile();
			return FALSE;
		}
	}

	csStream.CloseFile();

	return TRUE;
}
