// AgpmEventSpawn.cpp: implementation of the AgpmEventSpawn class.
//
//////////////////////////////////////////////////////////////////////

#include "AgpmEventSpawn.h"
#include "ApModuleStream.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgpmEventSpawn::AgpmEventSpawn()
{
	SetModuleName("AgpmEventSpawn");

	SetModuleData(sizeof(AgpdSpawn), AGPMSPAWN_DATA_SPAWN);
	SetModuleData(sizeof(AgpdSpawnGroup), AGPMSPAWN_DATA_SPAWN_GROUP);

	m_lMaxGroupID = 0;

	m_csGroups.SetCount(4000);	// Default Group Number
	
	m_nAttachIndexChar		= 0		;
	m_pcsApmEventManager	= NULL	;
	m_pcsAgpmCharacter		= NULL	;
}

AgpmEventSpawn::~AgpmEventSpawn()
{

}

INT32	AgpmEventSpawn::GetMaxGroupCount()
{
	return m_csGroups.GetObjectCount();
}
INT32	AgpmEventSpawn::GetGroupCount()
{
	return m_csGroups.GetCount();
}

BOOL	AgpmEventSpawn::OnAddModule()
{
	// 상위 Module들 가져온다.
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");

	if (!m_pcsApmEventManager || !m_pcsAgpmCharacter)
		return FALSE;

	m_nAttachIndexChar = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdSpawnADChar), NULL, NULL);
	if (m_nAttachIndexChar < 0)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateActionStatus(CBUpdateCharStatus, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
		return FALSE;

	// Event Manager에 Event를 등록한다.
	if (!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_SPAWN, CBEventConstructor, CBEventDestructor, NULL, CBStreamWrite, CBStreamRead, this))
		return FALSE;

	if (!AddStreamCallback(AGPMSPAWN_DATA_SPAWN_GROUP, CBStreamReadGroup, CBStreamWriteGroup, this))
		return FALSE;

	return TRUE;
}

BOOL	AgpmEventSpawn::OnInit()
{
	if (!m_csGroups.InitializeObject(sizeof(AgpdSpawnGroup *), m_csGroups.GetCount()))
		return FALSE;

	return TRUE;
}

BOOL	AgpmEventSpawn::OnDestroy()
{
	INT32	lIndex = 0;

	for (AgpdSpawnGroup *pcsSpawnGroup = GetGroupSequence(&lIndex); pcsSpawnGroup; pcsSpawnGroup = GetGroupSequence(&lIndex))
	{
		RemoveGroup(pcsSpawnGroup);
	}

	return TRUE;
}

/*
BOOL	AgpmEventSpawn::CBUpdateIdle(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventSpawn *	pThis		= (AgpmEventSpawn *) pClass;
	ApdEvent *			pstEvent	= (ApdEvent *) pData;
	UINT32				ulClockCount= (UINT32) pCustData;

	return pThis->ProcessSpawn(pstEvent, ulClockCount);
}
*/

VOID	AgpmEventSpawn::SetMaxGroup(INT32 lMaxGroup)
{
	m_csGroups.SetCount(lMaxGroup);
}

BOOL	AgpmEventSpawn::CBStreamWrite(PVOID pData, PVOID pClass, PVOID pCustData)
{

	return TRUE;
}

BOOL	AgpmEventSpawn::CBStreamRead(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventSpawn *		pThis		= (AgpmEventSpawn *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	AgpdSpawn *				pstSpawn	= (AgpdSpawn *) pstEvent->m_pvData;
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;
	const CHAR *			szValueName;

	if (!pstStream->ReadNextValue())
		return TRUE;

	szValueName = pstStream->GetValueName();
	if (strcmp(szValueName, AGPMSPAWN_INI_NAME_START))
		return TRUE;

	while (pstStream->ReadNextValue())
	{
		szValueName = pstStream->GetValueName();

		if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_NAME))
		{
			pstStream->GetValue(pstSpawn->m_szName, AGPDSPAWN_SPAWN_NAME_LENGTH);
		}
		else if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_CTID))
		{
/*			ASSERT(pstSpawn->m_stConfig.m_lSpawnChar < (AGPDSPAWN_MAX_CHAR_NUM - 1));

			pstSpawnChar = &pstSpawn->m_stConfig.m_astChar[pstSpawn->m_stConfig.m_lSpawnChar];
			++(pstSpawn->m_stConfig.m_lSpawnChar);

			pstSpawnItem = &pstSpawnChar->m_astItem[0];

			pstStream->GetValue(&pstSpawnChar->m_lCTID);*/
		}
		else if( !strcmp(szValueName, AGPMSPAWN_INI_NAME_AI_TID ) )
		{
//			pstStream->GetValue(&pstSpawnChar->m_lAITID );
		}
		else if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_SPAWN_RATE))
		{
//			pstStream->GetValue(&pstSpawnChar->m_lSpawnRate);
		}
		else if (!strcmp(szValueName, AGPMSPAWN_INI_DROPITEM_TEMPLATE))
		{
/*			// 마고자 (2004-04-19 오후 3:44:29) : 인덱스 오류인거같아서 수정.
			ASSERT( pstSpawnChar->m_lItem < AGPDSPAWN_MAX_ITEM_NUM );

			pstSpawnItem = &pstSpawnChar->m_astItem[pstSpawnChar->m_lItem];
			++(pstSpawnChar->m_lItem);

			pstStream->GetValue(pstSpawnItem->m_strDropItemTemplate, sizeof(pstSpawnItem->m_strDropItemTemplate) );*/
		}
		else if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_MAX_CHAR))
		{
//			pstStream->GetValue(&pstSpawn->m_stConfig.m_lMaxChar);
		}
		else if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_GROUPID))
		{
			INT32	lGroupID = 0;

			pstStream->GetValue(&lGroupID);

			if (pstEvent->m_pcsSource && lGroupID && 
				(pstEvent->m_pcsSource->m_eType == APBASE_TYPE_OBJECT || 
				pstEvent->m_pcsSource->m_eType == APBASE_TYPE_CHARACTER ||
				pstEvent->m_pcsSource->m_eType == APBASE_TYPE_ITEM))
//				ASSERT(pThis->AddSpawnToGroup(pThis->GetGroup(lGroupID), pstEvent));
				pThis->AddSpawnToGroup(pThis->GetGroup(lGroupID), pstEvent);
			else
				pstSpawn->m_lGroupID = lGroupID;
		}
		else if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_SUB_START))
		{
			if (!pstStream->EnumReadCallback(AGPMSPAWN_DATA_SPAWN, pstSpawn, pThis))
				return FALSE;
		}
		else if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_EVENT_START))
		{
			if (!pThis->m_pcsApmEventManager->StreamRead(NULL, &pstSpawn->m_stEvent, pstStream))
				return FALSE;
		}
		else if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_END))
			break;
	}

	return TRUE;
}

BOOL	AgpmEventSpawn::CBStreamWriteGroup(PVOID pData, ApModule *pClass, ApModuleStream *pstStream)
{
	AgpmEventSpawn *	pThis		= (AgpmEventSpawn *) pClass;
	AgpdSpawnGroup *	pstGroup	= (AgpdSpawnGroup *) pData;

	if (!pstStream->WriteValue(AGPMSPAWN_INI_NAME_GROUPID, pstGroup->m_lID))
		return FALSE;

	if (!pstStream->WriteValue(AGPMSPAWN_INI_NAME_NAME, pstGroup->m_szName))
		return FALSE;

	//if (!pstStream->WriteValue(AGPMSPAWN_INI_NAME_MIN_CHAR, pstGroup->m_lMinChar))
	//	return FALSE;

	//if (!pstStream->WriteValue(AGPMSPAWN_INI_NAME_MAX_CHAR, pstGroup->m_lMaxChar))
	//	return FALSE;

	//if (!pstStream->WriteValue(AGPMSPAWN_INI_NAME_INTERVAL, pstGroup->m_lInterval))
	//	return FALSE;

	return TRUE;
}

BOOL	AgpmEventSpawn::CBStreamReadGroup(PVOID pData, ApModule *pClass, ApModuleStream *pstStream)
{
	AgpmEventSpawn *	pThis		= (AgpmEventSpawn *) pClass;
	AgpdSpawnGroup *	pstGroup	= (AgpdSpawnGroup *) pData;
	const CHAR *		szValueName;

	while (pstStream->ReadNextValue())
	{
		szValueName = pstStream->GetValueName();

		if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_GROUPID))
		{
			pstStream->GetValue(&pstGroup->m_lID);
			if (pThis->m_lMaxGroupID < pstGroup->m_lID)
				pThis->m_lMaxGroupID = pstGroup->m_lID;
		}
		else if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_NAME))
		{
			pstStream->GetValue(pstGroup->m_szName, AGPDSPAWN_SPAWN_NAME_LENGTH);
		}
		//else if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_MIN_CHAR))
		//{
		//	pstStream->GetValue(&pstGroup->m_lMinChar);
		//}
		//else if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_MAX_CHAR))
		//{
		//	pstStream->GetValue(&pstGroup->m_lMaxChar);
		//}
		//else if (!strcmp(szValueName, AGPMSPAWN_INI_NAME_INTERVAL))
		//{
		//	pstStream->GetValue(&pstGroup->m_lInterval);
		//}
	}

	return TRUE;
}

INT16	AgpmEventSpawn::AttachSpawnData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, AGPMSPAWN_DATA_SPAWN, nDataSize, pfConstructor, pfDestructor);
}

INT16	AgpmEventSpawn::AttachSpawnGroupData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, AGPMSPAWN_DATA_SPAWN_GROUP, nDataSize, pfConstructor, pfDestructor);
}

AgpdSpawnGroup *	AgpmEventSpawn::CreateSpawnGroup()
{
	return (AgpdSpawnGroup *) CreateModuleData(AGPMSPAWN_DATA_SPAWN_GROUP);
}

AgpdSpawnGroup *	AgpmEventSpawn::AddSpawnGroup(CHAR *szName, BOOL bGenID)
{
	AgpdSpawnGroup *pstGroup;

	pstGroup = CreateSpawnGroup();
	if (!pstGroup)
		return FALSE;

	ZeroMemory(pstGroup->m_szName, sizeof(CHAR) * AGPDSPAWN_SPAWN_NAME_LENGTH);
	strncpy(pstGroup->m_szName, (const CHAR *) szName, AGPDSPAWN_SPAWN_NAME_LENGTH - 1);

	return AddSpawnGroup(pstGroup, bGenID);
}

AgpdSpawnGroup *	AgpmEventSpawn::AddSpawnGroup(AgpdSpawnGroup *pstGroup, BOOL bGenID)
{
	if (bGenID)
	{
		++m_lMaxGroupID;
		pstGroup->m_lID = m_lMaxGroupID;
	}

	return m_csGroups.AddGroup(pstGroup);
}

AgpdSpawnGroup *	AgpmEventSpawn::GetGroup(INT32 lSGID)
{
	return m_csGroups.GetGroup(lSGID);
}

AgpdSpawnGroup *	AgpmEventSpawn::GetGroup(char *pstrGroupName)
{
	return m_csGroups.GetGroup(pstrGroupName);
}

BOOL				AgpmEventSpawn::RemoveGroup(AgpdSpawnGroup *pstGroup)
{
	m_csGroups.RemoveGroup(pstGroup);

	return DestroyModuleData(pstGroup, AGPMSPAWN_DATA_SPAWN_GROUP);
}

BOOL				AgpmEventSpawn::RemoveGroup(INT32 lSGID)
{
	AgpdSpawnGroup *pstGroup;

	pstGroup = GetGroup(lSGID);
	if (!pstGroup)
		return FALSE;

	return RemoveGroup(pstGroup);
}

AgpdSpawnGroup *	AgpmEventSpawn::GetGroupSequence(INT32 *plIndex)
{
	AgpdSpawnGroup **pstGroup = (AgpdSpawnGroup **) m_csGroups.GetObjectSequence(plIndex);

	if (!pstGroup)
		return NULL;

	return  *pstGroup;
}

BOOL				AgpmEventSpawn::AddSpawnToGroup(AgpdSpawnGroup *pstGroup, ApdEvent *pstEvent)
{
	if (!pstGroup || !pstEvent || !pstEvent->m_pvData || pstEvent->m_eFunction != APDEVENT_FUNCTION_SPAWN)
		return FALSE;

	AgpdSpawn *			pstSpawn = (AgpdSpawn *) pstEvent->m_pvData;

	if (pstSpawn->m_pstParent == pstGroup)
		return FALSE;

	if (pstSpawn->m_pstParent)
		RemoveSpawnFromGroup(pstEvent);

	pstSpawn->m_lGroupID = pstGroup->m_lID;
	pstSpawn->m_pstParent = pstGroup;
	pstSpawn->m_pstNext = pstGroup->m_pstChild;	
	pstGroup->m_pstChild = pstEvent;

	return TRUE;
}

BOOL				AgpmEventSpawn::RemoveSpawnFromGroup(ApdEvent *pstEvent)
{
	if (!pstEvent || !pstEvent->m_pvData || pstEvent->m_eFunction != APDEVENT_FUNCTION_SPAWN)
		return FALSE;

	AgpdSpawn *			pstSpawn = (AgpdSpawn *) pstEvent->m_pvData;
	AgpdSpawn *			pstSpawnTemp;
	ApdEvent *			pstEventTemp;
	AgpdSpawnGroup *	pstGroup = pstSpawn->m_pstParent;

	if (!pstSpawn->m_pstParent)
		return FALSE;

	pstEventTemp = pstGroup->m_pstChild;
	while (pstEventTemp)
	{
		if (((AgpdSpawn *)pstEventTemp->m_pvData)->m_pstNext == pstEvent)
			break;

		pstEventTemp = ((AgpdSpawn *)pstEventTemp->m_pvData)->m_pstNext;
	}

	if (pstEventTemp)
	{
		pstSpawnTemp = (AgpdSpawn *)pstEventTemp->m_pvData;

		if (pstEventTemp == pstGroup->m_pstChild)
			pstGroup->m_pstChild = pstSpawn->m_pstNext;
		else
			pstSpawnTemp->m_pstNext = pstSpawn->m_pstNext;
	}

	pstSpawn->m_pstParent = NULL;
	pstSpawn->m_pstNext = NULL;

	return TRUE;
}

BOOL				AgpmEventSpawn::ChangeGroupName(AgpdSpawnGroup *pstGroup, CHAR *szName)
{
	if (m_csGroups.GetGroup(szName))
		return FALSE;

	m_csGroups.RemoveGroup(pstGroup);

	ZeroMemory(pstGroup->m_szName, sizeof(CHAR) * AGPDSPAWN_SPAWN_NAME_LENGTH);
	strncpy(pstGroup->m_szName, (const CHAR *) szName, AGPDSPAWN_SPAWN_NAME_LENGTH - 1);

	if (!m_csGroups.AddGroup(pstGroup))
		return FALSE;

	return TRUE;
}

AgpdSpawnADChar *	AgpmEventSpawn::GetCharacterData(AgpdCharacter *pcsCharacter)
{
	return (AgpdSpawnADChar *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nAttachIndexChar, pcsCharacter);
}

BOOL				AgpmEventSpawn::StreamWriteGroup(CHAR *szFile)
{
	ApModuleStream		csStream;
	INT32				lIndex = 0;
	CHAR				szSGID[32];
	AgpdSpawnGroup*		pstSpawGroup;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// 등록된 모든 Character Template에 대해서...
	for (pstSpawGroup = GetGroupSequence(&lIndex); pstSpawGroup; pstSpawGroup = GetGroupSequence(&lIndex))
	{
		sprintf(szSGID, "%d", pstSpawGroup->m_lID);

		// TID로 Section을 설정하고
		csStream.SetSection(szSGID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGPMSPAWN_DATA_SPAWN_GROUP, pstSpawGroup, this))
			return FALSE;
	}

	csStream.Write(szFile);

	return TRUE;
}

BOOL				AgpmEventSpawn::StreamReadGroup(CHAR *szFile)
{
	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	INT32				lSGID;
	AgpdSpawnGroup *	pstGroup;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(szFile);

	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for (i = 0; i < nNumKeys; i++)
	{
		// Section Name은 TID 이다.
		lSGID = atoi(csStream.ReadSectionName(i));

		// Template을 추가한다.
		pstGroup = CreateSpawnGroup();
		if (!pstGroup)
			return FALSE;

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMSPAWN_DATA_SPAWN_GROUP, pstGroup, this))
			return FALSE;

		if (!AddSpawnGroup(pstGroup, FALSE))
		{
			RemoveGroup(pstGroup);
		}
	}

	return TRUE;
}

BOOL				AgpmEventSpawn::CBUpdateCharStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventSpawn *	pThis			= (AgpmEventSpawn *) pClass;
	AgpdCharacter *		pcsCharacter	= (AgpdCharacter *) pData;
	INT16 *				pnOldStatus		= (INT16 *) pCustData;

	if (*pnOldStatus != AGPDCHAR_STATUS_DEAD && pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		BOOL	bIsRespawnMonster	= TRUE;
		pThis->EnumCallback(AGPMSPAWN_CB_IS_RESPAWN_MONSTER, pcsCharacter, &bIsRespawnMonster);

		if (bIsRespawnMonster)
		{
			//pThis->RemoveMonsterFromSpawnList(pcsCharacter);
			pThis->AddRespawnTime(pcsCharacter);
		}
	}

	return TRUE;
}

BOOL		AgpmEventSpawn::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventSpawn *	pThis			= (AgpmEventSpawn *) pClass;
	AgpdCharacter *		pcsCharacter	= (AgpdCharacter *) pData;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacter"));

	BOOL	bIsRespawnMonster	= TRUE;
	pThis->EnumCallback(AGPMSPAWN_CB_IS_RESPAWN_MONSTER, pcsCharacter, &bIsRespawnMonster);

	if (pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD || !bIsRespawnMonster)
	{
		pThis->RemoveMonsterFromSpawnList(pcsCharacter);
	}

	return TRUE;
}

BOOL AgpmEventSpawn::RemoveMonsterFromSpawnList(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || m_pcsAgpmCharacter->IsStatusWasTamed(pcsCharacter))
		return FALSE;

	AgpdSpawn *pstSpawn = GetCharacterData(pcsCharacter)->m_pstSpawn;

	if (pstSpawn)
	{
		BOOL	bRemove	= FALSE;

		if (pstSpawn->m_Mutex.WLock())
		{
			for (int i = 0; i < AGPDSPAWN_MAX_SPAWNED_CHAR; ++i)
			{
				if (pstSpawn->m_stSpawnCharacters[i].m_lID == pcsCharacter->m_lID)
				{
					pstSpawn->m_stSpawnCharacters[i].m_lID = AP_INVALID_CID;
					pstSpawn->m_stSpawnCharacters[i].m_ulRespawnTime = 0;

					bRemove	= TRUE;

					break;
				}
			}

			pstSpawn->m_Mutex.Release();
		}
	}

	return TRUE;
}

BOOL AgpmEventSpawn::AddRespawnTime(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdSpawn *pcsSpawn = GetCharacterData(pcsCharacter)->m_pstSpawn;

	if (!pcsSpawn)
		return FALSE;

	if (pcsSpawn->m_Mutex.WLock())
	{
		for(INT32 i = 0; i < AGPDSPAWN_MAX_SPAWNED_CHAR; ++i)
		{
			if(pcsSpawn->m_stSpawnCharacters[i].m_lID == pcsCharacter->m_lID)
			{
				pcsSpawn->m_stSpawnCharacters[i].m_lID = 0;
				pcsSpawn->m_stSpawnCharacters[i].m_ulRespawnTime = CTime::GetCurrentTime() + CTimeSpan(0, 0, 0, pcsSpawn->m_lSpawnInterval);
				break;
			}
		}

		pcsSpawn->m_Mutex.Release();
	}

	return TRUE;
}

BOOL				AgpmEventSpawn::CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventSpawn *		pThis = (AgpmEventSpawn *) pClass;
	ApdEvent *				pstDstEvent = (ApdEvent *) pData;
	ApdEvent *				pstSrcEvent = (ApdEvent *) pCustData;
	AgpdSpawn *				pstDstSpawn;
	AgpdSpawn *				pstSrcSpawn;

	pstDstEvent->m_pvData = pThis->CreateModuleData(AGPMSPAWN_DATA_SPAWN);
	if (!pstDstEvent->m_pvData)
		return FALSE;

	((AgpdSpawn *) pstDstEvent->m_pvData)->m_Mutex.Init();

	if (pstSrcEvent)
	{
		pstDstSpawn = (AgpdSpawn *) pstDstEvent->m_pvData;
		pstSrcSpawn = (AgpdSpawn *) pstSrcEvent->m_pvData;

		memcpy(pstDstEvent->m_pvData, pstSrcEvent->m_pvData, pThis->GetDataSize(AGPMSPAWN_DATA_SPAWN));

		if (pstSrcSpawn->m_lGroupID)
			pThis->AddSpawnToGroup(pThis->GetGroup(pstSrcSpawn->m_lGroupID), pstDstEvent);
	}

	return TRUE;
}

BOOL				AgpmEventSpawn::CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventSpawn *		pThis = (AgpmEventSpawn *) pClass;
	ApdEvent *				pstEvent = (ApdEvent *) pData;

	if (pstEvent->m_pvData)
		((AgpdSpawn *) pstEvent->m_pvData)->m_Mutex.Destroy();

	pThis->DestroyModuleData(pstEvent->m_pvData, AGPMSPAWN_DATA_SPAWN);

	return TRUE;
}

BOOL				AgpmEventSpawn::LoadSpawnInfoFromExcel( char *pstrFileName)
{
	if (!pstrFileName || !pstrFileName[0])
		return FALSE;

	if (!m_csAuExcelTxtLib.OpenExcelFile(pstrFileName, TRUE))
		return FALSE;

	INT32	lMaxRow			= m_csAuExcelTxtLib.GetRow();
	INT32	lMaxColumn		= m_csAuExcelTxtLib.GetColumn();

	AgpdSpawnGroup	*pcsSpawnGroup	= NULL;
	AgpdSpawn* pcsSpawn = NULL;

	for (INT32 lCurrentRow = 1; lCurrentRow < lMaxRow; ++lCurrentRow)
	{
		CHAR* szGroupName		= m_csAuExcelTxtLib.GetData(0, lCurrentRow);
		CHAR* szName			= m_csAuExcelTxtLib.GetData(1, lCurrentRow);
		CHAR* szTotalMobCount	= m_csAuExcelTxtLib.GetData(2, lCurrentRow);
		CHAR* szMaxMobCount		= m_csAuExcelTxtLib.GetData(2, lCurrentRow);
		CHAR* szSpawntime		= m_csAuExcelTxtLib.GetData(3, lCurrentRow);
		CHAR* szBobTID			= m_csAuExcelTxtLib.GetData(4, lCurrentRow);
		CHAR* szRate			= m_csAuExcelTxtLib.GetData(7, lCurrentRow);
		CHAR* szAITID			= m_csAuExcelTxtLib.GetData(8, lCurrentRow);
		CHAR* szSiegeWarType	= m_csAuExcelTxtLib.GetData(10, lCurrentRow);

		if(szGroupName)
		{
			pcsSpawnGroup = GetGroup(szGroupName);

			if(!pcsSpawnGroup)
			{
				CHAR strLog[256] = { 0, };
				sprintf_s(strLog, sizeof(strLog), "SpawnGroup get fail - %s", szGroupName);
				AuLogFile_s("SpawnError.txt", strLog);
				continue;
			}

			for ( ApdEvent* pcsEvent = pcsSpawnGroup->m_pstChild;
				pcsEvent;
				pcsEvent = pcsEvent->m_pvData ? ((AgpdSpawn *) pcsEvent->m_pvData)->m_pstNext : NULL )
			{
				if (pcsEvent->m_pvData && strcmp(((AgpdSpawn *) pcsEvent->m_pvData)->m_szName, szName) == 0)
				{
					pcsSpawn = (AgpdSpawn *) pcsEvent->m_pvData;
					
					if(!pcsSpawn)
					{
						CHAR strLog[256] = { 0, };
						sprintf_s(strLog, sizeof(strLog), "Spawn get fail - %s", szName);
						AuLogFile_s("SpawnError.txt", strLog);
						continue;
					}

					if (szTotalMobCount)
					{
						pcsSpawn->m_lTotalCharacter = atoi(szTotalMobCount);
					}

					if (szSiegeWarType)
					{
						pcsSpawn->m_lSiegeWarType = (AgpdSpawnSiegeWarType)atoi(szSiegeWarType);
					}

					if (szSpawntime) 
					{
						pcsSpawn->m_lSpawnInterval = atoi(szSpawntime) / 1000;
					}

					for(INT32 i = 0; i < AGPDSPAWN_MAX_CHAR_NUM; ++i)
					{
						if(pcsSpawn->m_stSpawnConfig[i].m_lTID != 0)
							continue;

						pcsSpawn->m_stSpawnConfig[i].m_lTID				= (szBobTID)		? atoi(szBobTID)		: 0;
						pcsSpawn->m_stSpawnConfig[i].m_lAITID			= (szAITID)			? atoi(szAITID)			: 0;
						pcsSpawn->m_stSpawnConfig[i].m_lSpawnRate		= (szRate)			? atoi(szRate)			: 0;
						pcsSpawn->m_stSpawnConfig[i].m_lMaxMobCount		= (szMaxMobCount)	? atoi(szMaxMobCount)	: 0;

						break;
					}
				}
			}
		}
		else
		{
			if(pcsSpawn)
			{
				for(INT32 i = 0; i < AGPDSPAWN_MAX_CHAR_NUM; ++i)
				{
					if(pcsSpawn->m_stSpawnConfig[i].m_lTID != 0)
						continue;

					pcsSpawn->m_stSpawnConfig[i].m_lTID				= (szBobTID)		? atoi(szBobTID)		: 0;
					pcsSpawn->m_stSpawnConfig[i].m_lAITID			= (szAITID)			? atoi(szAITID)			: 0;
					pcsSpawn->m_stSpawnConfig[i].m_lSpawnRate		= (szRate)			? atoi(szRate)			: 0;
					pcsSpawn->m_stSpawnConfig[i].m_lMaxMobCount		= (szMaxMobCount)	? atoi(szMaxMobCount)	: 0;

					break;
				}
			}
		}
	}

	m_csAuExcelTxtLib.CloseFile();

	return TRUE;

}

// 2005.07.21. steeple.
// AgpmEventSpawn 에서도 AgsmEventSpawn 의 Spawn 콜백을 불러줄 수 있게끔.
BOOL AgpmEventSpawn::SetCallbackSpawn(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSPAWN_CB_SPAWN, pfCallback, pClass);
}

BOOL AgpmEventSpawn::SetCallbackIsRespawnMonster(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSPAWN_CB_IS_RESPAWN_MONSTER, pfCallback, pClass);
}



AgpdSpawn* AgpmEventSpawn::GetSpawnByMonsterTID(INT32 lTID, INT32* plSpawnIndex)
{
	if(lTID == AP_INVALID_CID)
		return NULL;

	AgpdSpawnGroup* pstGroup = NULL;
	AgpdSpawn* pstSpawn = NULL;
	INT32 lIndex = 0;

	for(pstGroup = GetGroupSequence(&lIndex); pstGroup; pstGroup = GetGroupSequence(&lIndex))
	{
		for(ApdEvent* pstEvent = pstGroup->m_pstChild; pstEvent; pstEvent = ((AgpdSpawn*)pstEvent->m_pvData)->m_pstNext)
		{
			pstSpawn = (AgpdSpawn*)pstEvent->m_pvData;
			if(!pstSpawn)
				break;

			for(INT32 i = 0; i < AGPDSPAWN_MAX_CHAR_NUM; i++)
			{
				if(pstSpawn->m_stSpawnConfig[i].m_lTID == 0)
					break;

				// 해당 TID 를 찾았다. 바로 리턴
				if(pstSpawn->m_stSpawnConfig[i].m_lTID == lTID)
				{
					if(plSpawnIndex)
						*plSpawnIndex = i;

					return pstSpawn;
				}
			}			
		}
	}

	return NULL;
}

AgpdSpawn* AgpmEventSpawn::FindSpawnData(ApdEvent* pcsEvent, CHAR* strName)
{
	while (pcsEvent)
	{
		AgpdSpawn* pcsAgpdSpawn = (AgpdSpawn*)pcsEvent->m_pvData;
		if(!pcsAgpdSpawn) break;

		if (strcmp(pcsAgpdSpawn->m_szName, strName) == 0) return pcsAgpdSpawn;

		pcsEvent = pcsAgpdSpawn->m_pstNext;
	}

	return NULL;
}
