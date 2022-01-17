/******************************************************************************
Module:  AgsmSkillManager.cpp
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 26
******************************************************************************/

#include "AgsmSkillManager.h"
#include "ApMemoryTracker.h"

extern	INT32	g_alReceivedSkillPoint[2][3];

AgsmSkillManager::AgsmSkillManager()
{
	SetModuleName("AgsmSkillManager");
}

AgsmSkillManager::~AgsmSkillManager()
{
}

BOOL AgsmSkillManager::OnAddModule()
{
	m_pagpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pagpmCharacter	= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pagpmSkill		= (AgpmSkill *)			GetModule("AgpmSkill");
	m_pagpmProduct		= (AgpmProduct *)		GetModule("AgpmProduct");
	
	//m_pagsmDBStream		= (AgsmDBStream *)		GetModule("AgsmDBStream");
	m_pagsmCharacter	= (AgsmCharacter *)		GetModule("AgsmCharacter");
	m_pagsmCharManager	= (AgsmCharManager *)	GetModule("AgsmCharManager");
	m_pagsmSkill		= (AgsmSkill *)			GetModule("AgsmSkill");
	
	// JNY TODO : Relay 서버 개발을 위해 m_pagsmDBStream를 체크하는 부분을 
	// 잠시 삭제합니다. 
	// 2004.2.16
	// 다시 복구 하거나 DB모듈 변경에 의한 변경작업을 해야합니다.

	if (!m_pagpmFactors ||
		!m_pagpmCharacter ||
		!m_pagpmSkill ||
/*		!m_pagpmProduct ||	*/
		/*!m_pagsmDBStream ||*/
		!m_pagsmSkill ||
		!m_pagsmCharacter ||
		!m_pagsmCharManager)
		return FALSE;

	/*
	if (!m_pagsmDBStream->SetCallbackMaxIDResult(CBStreamDB, this))
		return FALSE;
	*/

	if (!m_pagsmCharManager->SetCallbackCreateChar(CBCreateCharacter, this))
		return FALSE;
	if (!m_pagsmCharManager->SetCallbackDeleteChar(CBDeleteCharacter, this))
		return FALSE;

	if (!m_pagpmSkill->SetCallbackLearnSkill(CBLearnSkill, this))
		return FALSE;
	if (!m_pagpmSkill->SetCallbackForgetSkill(CBForgetSkill, this))
		return FALSE;
	if (!m_pagpmSkill->SetCallbackRemoveID(CBRemoveSkillID, this))
		return FALSE;
	/*
	if (!m_pagpmSkill->SetCallbackDeleteSkill(CBDeleteSkill, this))
		return FALSE;
	*/

	if (!m_pagsmSkill->SetCallbackGetNewSkillID(CBGetNewSkillID, this))
		return FALSE;

	if (!m_pagsmCharManager->SetCallbackEnterGameWorld(CBEnterGameWorld, this))
		return FALSE;
	if (!m_pagsmCharManager->SetCallbackSetCharLevel(CBSetCharLevel, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmSkillManager::OnInit()
{
	return TRUE;
}

BOOL AgsmSkillManager::OnDestroy()
{
	return TRUE;
}

BOOL AgsmSkillManager::InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag, INT32 lRemoveIDQueueSize)
{
	return m_csGenerateID.Initialize(ulStartValue, ulServerFlag, nSizeServerFlag, lRemoveIDQueueSize, TRUE);
}

INT32 AgsmSkillManager::GenerateID()
{
	return m_csGenerateID.GetID();
}

AgpdSkill* AgsmSkillManager::CreateSkill(ApBase *pcsOwnerBase, INT32 lSkillTID)
{
	if (!pcsOwnerBase || lSkillTID == AP_INVALID_SKILLID)
		return NULL;

	AgpdSkill *pcsSkill = m_pagpmSkill->AddSkill(GenerateID(), lSkillTID);
	AgsdSkill *pcsAgsdSkill;

	if (!pcsSkill)
		return NULL;

	pcsAgsdSkill = m_pagsmSkill->GetADSkill( pcsSkill );

	if( !pcsAgsdSkill )
		return NULL;

	pcsSkill->m_pcsBase = pcsOwnerBase;

	m_pagpmSkill->AddOwnSkillList(pcsSkill);

	return pcsSkill;
}

BOOL AgsmSkillManager::RemoveSkill(AgpdSkill *pcsSkill)
{
	if (!pcsSkill)
		return FALSE;

	m_pagpmSkill->RemoveOwnSkillList(pcsSkill);

	return m_pagpmSkill->RemoveSkill(pcsSkill->m_lID);
}

BOOL AgsmSkillManager::CBRemoveSkillID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSkillManager		*pThis				= (AgsmSkillManager *)		pClass;
	AgpdSkill				*pcsSkill			= (AgpdSkill *)				pData;

	pThis->m_csGenerateID.AddRemoveID(pcsSkill->m_lID);

	return TRUE;
}

/*
BOOL AgsmSkillManager::CBDeleteSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSkillManager	*pThis			= (AgsmSkillManager *)	pClass;
	AgpdSkill			*pcsSkill		= (AgpdSkill *)			pData;

	return pThis->m_pagsmSkill->StreamDeleteDB(pcsSkill, NULL, NULL);
}
*/

BOOL AgsmSkillManager::CBCreateCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSkillManager	*pThis			= (AgsmSkillManager *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)		pData;

	AgpdSkillTemplateAttachData *pcsTemplateAttachData = pThis->m_pagpmSkill->GetAttachSkillTemplateData(pcsCharacter->m_pcsCharacterTemplate);
	if (!pcsTemplateAttachData)
		return FALSE;

	/*
	// pcsCharacter의 템플릿에 명시되어 있는 default skill을 생성해준다.
	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
	{
		if (!strlen(pcsTemplateAttachData->m_aszUsableSkillTName[i]))
			break;

		AgpdSkillTemplate *pcsSkillTemplate = pThis->m_pagpmSkill->GetSkillTemplate(pcsTemplateAttachData->m_aszUsableSkillTName[i]);
		if (!pcsSkillTemplate)
			continue;

		AgpdSkill *pcsSkill = pThis->CreateSkill(pcsCharacter, pcsSkillTemplate->m_lID);
		if (!pcsSkill)
			continue;

		// test로 skill level을 다 30으로 준다.
		///////////////////////////////////////////////////////////////////////////////////////
		AgpdFactorDIRT	*pcsFactorDIRT = (AgpdFactorDIRT *) pThis->m_pagpmFactors->SetFactor(&pcsSkill->m_csFactor, NULL, AGPD_FACTORS_TYPE_DIRT);
		if (pcsFactorDIRT)
		{
			INT32	lCharLevel = pThis->m_pagpmFactors->GetLevel(&pcsCharacter->m_csFactor);
			if (lCharLevel <= 0)
				lCharLevel = 1;

			pcsFactorDIRT->lValue[AGPD_FACTORS_DIRT_TYPE_SKILL_LEVEL] = lCharLevel;
		}
		// test로 skill level을 다 10으로 준다.
		///////////////////////////////////////////////////////////////////////////////////////

		pThis->m_pagpmSkill->InitSkill(pcsSkill);
	}
	*/

	return TRUE;
}

BOOL AgsmSkillManager::CBDeleteCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSkillManager	*pThis			= (AgsmSkillManager *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)		pData;

	// pcsCharacter가 가지고 있는 skill 을 모두 삭제한다.
	//
	//
	//

	return TRUE;
}

BOOL AgsmSkillManager::CBGetNewSkillID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSkillManager	*pThis			= (AgsmSkillManager *)	pClass;
	INT32				*plSkillID		= (INT32 *)				pData;

	*plSkillID	= pThis->m_csGenerateID.GetID();

	return TRUE;
}

BOOL AgsmSkillManager::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSkillManager		*pThis			= (AgsmSkillManager *)		pClass;
	AgpdCharacter			*pcsCharacter	= (AgpdCharacter *)			pData;

	// 이 함수 자체가 테스트용이다.
	// 현재 스킬이 로그인 서버에서 제대로 넘어오지 않기 땜시 여기서 일단 걍 무조건 만들어준다.

	return pThis->CBCreateCharacter(pcsCharacter, pThis, NULL);
}

BOOL AgsmSkillManager::CBSetCharLevel(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSkillManager	*pThis			= (AgsmSkillManager *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)		pData;

	pThis->m_pagpmSkill->AdjustSkillPoint(pcsCharacter);
	pThis->m_pagpmSkill->AdjustHeroicPoint(pcsCharacter);

	return TRUE;

	/*
	INT32				lLevel			= *(INT32 *)			pCustData;

	if (lLevel < 1 || lLevel > AGPMCHAR_MAX_LEVEL)
		return FALSE;

	INT32				lTotalSkillPoint	= 0;
	INT32				lCurrentSkillPoint	= 0;

	INT32				lIndexTable	= 0;

	// lTotalSkillPoint에 이 레벨에서 얻을 수 있는 최대 SP를 계산한다.
	//		- Point는 50레벨까지는 2점을, 51레벨부터 100레벨까지는 3점을 얻게 된다.
	//////////////////////////////////////////////////////////////////////////

	if (lLevel > 50)
	{
		lTotalSkillPoint = 50 * 2 + ((lLevel - 1) - 50) * 3;
		lIndexTable	= 1;
	}
	else
		lTotalSkillPoint = (lLevel - 1) * 2;

	INT32				lMaxExp				= 0;
	pThis->m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxExp, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP);

	INT32				lCurrentExp			= 0;
	pThis->m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentExp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP);

	if (lMaxExp > 0)
	{
		INT32	lCurrentExpPercent	= (INT32) (lCurrentExp / (lMaxExp + 0.0) * 100);

		if (g_alReceivedSkillPoint[lIndexTable][0] <= lCurrentExpPercent)
		{
			for (int i = 1; i < AGSMCHARACTER_MAX_RECEIVED_SKILL_POINT; ++i)
			{
				if (g_alReceivedSkillPoint[lIndexTable][i] == 0)
				{
					i = AGSMCHARACTER_MAX_RECEIVED_SKILL_POINT;
					break;
				}

				if (lCurrentExpPercent < g_alReceivedSkillPoint[lIndexTable][i])
					break;
			}

			if (i < AGSMCHARACTER_MAX_RECEIVED_SKILL_POINT)
			{
				lTotalSkillPoint += i;

				AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
				if (pcsAgsdCharacter)
				{
					pcsAgsdCharacter->m_lReceivedSkillPoint = i;
				}
			}
		}
	}

	// 현재 pcsCharacter의 SP가 얼마인지 얻어온다.
	//////////////////////////////////////////////////////////////////////////

	lCurrentSkillPoint	= pThis->m_pagpmCharacter->GetSkillPoint(pcsCharacter);

	return pThis->m_pagpmCharacter->UpdateSkillPoint(pcsCharacter, lTotalSkillPoint - lCurrentSkillPoint);
	*/
}

/*
BOOL AgsmSkillManager::SetCallbackDBStreamInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSKILLMANAGER_CB_DB_STREAM_INSERT, pfCallback, pClass);
}

BOOL AgsmSkillManager::SetCallbackDBStreamDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSKILLMANAGER_CB_DB_STREAM_DELETE, pfCallback, pClass);
}

BOOL AgsmSkillManager::SetCallbackDBStreamSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSKILLMANAGER_CB_DB_STREAM_SELECT, pfCallback, pClass);
}

BOOL AgsmSkillManager::SetCallbackDBStreamUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSKILLMANAGER_CB_DB_STREAM_UPDATE, pfCallback, pClass);
}
*/

/*
BOOL AgsmSkillManager::StreamInsertDB(CHAR *szServerGroup, CHAR *szServerName, UINT64 ulMaxDBID, ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	if (!szServerGroup || !szServerName || !szServerGroup[0] || !szServerName[0])
		return FALSE;

	pstAgsmDBOperationArg	pstArg = (pstAgsmDBOperationArg) GlobalAlloc(GMEM_FIXED, sizeof(stAgsmDBOperationArg));
	if (!pstArg)	return FALSE;

	ZeroMemory(pstArg, sizeof(stAgsmDBOperationArg));

	pstArg->nDataType	= AGSMDB_DATATYPE_MAXSKILLID;
	pstArg->nOperation	= AGSMDB_OPERATION_INSERT;
	pstArg->pfCallback	= pfCallback;
	pstArg->pClass		= pClass;

	CHAR	*szQuery = pstArg->szQuery;

	sprintf(szQuery, "INSERT INTO GENERATED_MAX_SKILL_ID (SERVERNAME, GROUPNAME, MAXID) VALUES ('%s', '%s', %8d)",
								szServerName,
								szServerGroup,
								ulMaxDBID);

	return m_pagsmDBStream->ExecuteQuery(pstArg, __FILE__, __LINE__ );
}

BOOL AgsmSkillManager::StreamSelectDB(CHAR *szServerGroup, CHAR *szServerName, ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	if (!szServerGroup || !szServerName || !szServerGroup[0] || !szServerName[0])
		return FALSE;

	pstAgsmDBOperationArg	pstArg = (pstAgsmDBOperationArg) GlobalAlloc(GMEM_FIXED, sizeof(stAgsmDBOperationArg));
	if (!pstArg)	return FALSE;

	ZeroMemory(pstArg, sizeof(stAgsmDBOperationArg));

	pstArg->nDataType	= AGSMDB_DATATYPE_MAXSKILLID;
	pstArg->nOperation	= AGSMDB_OPERATION_SELECT;
	pstArg->pfCallback	= pfCallback;
	pstArg->pClass		= pClass;

	CHAR	*szQuery = pstArg->szQuery;

	sprintf(szQuery, "SELECT MAXID FROM GENERATED_MAX_SKILL_ID WHERE SERVERNAME='%s' AND GROUPNAME='%s'", szServerName, szServerGroup);

	return m_pagsmDBStream->ExecuteQuery(pstArg, __FILE__, __LINE__ );
}

BOOL AgsmSkillManager::StreamUpdateDB(CHAR *szServerGroup, CHAR *szServerName, UINT64 ulMaxDBID, ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	if (!szServerGroup || !szServerName || !szServerGroup[0] || !szServerName[0])
		return FALSE;

	pstAgsmDBOperationArg	pstArg = (pstAgsmDBOperationArg) GlobalAlloc(GMEM_FIXED, sizeof(stAgsmDBOperationArg));
	if (!pstArg)	return FALSE;

	ZeroMemory(pstArg, sizeof(stAgsmDBOperationArg));

	pstArg->nDataType	= AGSMDB_DATATYPE_MAXSKILLID;
	pstArg->nOperation	= AGSMDB_OPERATION_UPDATE;
	pstArg->pfCallback	= pfCallback;
	pstArg->pClass		= pClass;

	CHAR	*szQuery = pstArg->szQuery;

	sprintf(szQuery, "UPDATE GENERATED_MAX_SKILL_ID SET MAXID=%8d WHERE SERVERNAME='%s' AND GROUPNAME='%s'", 
						ulMaxDBID,
						szServerName,
						szServerGroup);

	return m_pagsmDBStream->ExecuteQuery(pstArg, __FILE__, __LINE__ );
}

BOOL AgsmSkillManager::StreamDeleteDB(CHAR *szServerGroup, CHAR *szServerName, ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	if (!szServerGroup || !szServerName || !szServerGroup[0] || !szServerName[0])
		return FALSE;

	pstAgsmDBOperationArg	pstArg = (pstAgsmDBOperationArg) GlobalAlloc(GMEM_FIXED, sizeof(stAgsmDBOperationArg));
	if (!pstArg)	return FALSE;

	ZeroMemory(pstArg, sizeof(stAgsmDBOperationArg));

	pstArg->nDataType	= AGSMDB_DATATYPE_MAXSKILLID;
	pstArg->nOperation	= AGSMDB_OPERATION_DELETE;
	pstArg->pfCallback	= pfCallback;
	pstArg->pClass		= pClass;

	CHAR	*szQuery = pstArg->szQuery;

	sprintf(szQuery, "DELETE FROM GENERATED_MAX_SKILL_ID WHERE SERVERNAME='%s' AND GROUPNAME='%s'", szServerName, szServerGroup);

	return m_pagsmDBStream->ExecuteQuery(pstArg, __FILE__, __LINE__ );
}

BOOL AgsmSkillManager::CBStreamDB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSkillManager		*pThis			= (AgsmSkillManager *)		pClass;
	COLEDB					*pcOLEDB		= (COLEDB *)				pData;
	pstAgsmDBOperationArg	pstOperationArg	= (pstAgsmDBOperationArg)	pCustData;

	if (pstOperationArg->nDataType != AGSMDB_DATATYPE_MAXSKILLID)
		return FALSE;

	switch (pstOperationArg->nOperation) {
	case AGSMDB_OPERATION_INSERT:
		{
			if (pstOperationArg->pfCallback && pstOperationArg->pClass)
				pstOperationArg->pfCallback(&pstOperationArg->lID, pstOperationArg->pClass, &pstOperationArg->bSuccess);
		}
		break;

	case AGSMDB_OPERATION_SELECT:
		{
			if (pstOperationArg->bSuccess)
			{
				UINT64	ullDBID	= 0;

				if (pcOLEDB->GetQueryResult(0) != NULL)
					ullDBID	= (UINT64) _atoi64(pcOLEDB->GetQueryResult(0));

				if (ullDBID > 0)
					pThis->m_csGenerateID64.Initialize(ullDBID, 0);
				else
					pstOperationArg->bSuccess = FALSE;
			}

			if (pstOperationArg->pfCallback && pstOperationArg->pClass)
				pstOperationArg->pfCallback(&pstOperationArg->lID, pstOperationArg->pClass, &pstOperationArg->bSuccess);
		}
		break;

	case AGSMDB_OPERATION_UPDATE:
		{
			if (pstOperationArg->pfCallback && pstOperationArg->pClass)
				pstOperationArg->pfCallback(&pstOperationArg->lID, pstOperationArg->pClass, &pstOperationArg->bSuccess);
		}
		break;

	case AGSMDB_OPERATION_DELETE:
		{
			if (pstOperationArg->pfCallback && pstOperationArg->pClass)
				pstOperationArg->pfCallback(&pstOperationArg->lID, pstOperationArg->pClass, &pstOperationArg->bSuccess);
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}
*/

BOOL AgsmSkillManager::CBLearnSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData || !((PVOID *) pCustData)[0] || !((PVOID *) pCustData)[2])
		return FALSE;

	AgsmSkillManager	*pThis				= (AgsmSkillManager *)	pClass;
	ApBase				*pcsBase			= (ApBase *)			pData;
	CHAR				*szSkillName		= (CHAR *)				((PVOID *) pCustData)[0];
//	INT32				lMasteryIndex		= *(INT32 *)			((PVOID *) pCustData)[2];
	INT32				lActivedSkillPoint	= *(INT32 *)			((PVOID *) pCustData)[2];

	// pcsCharacter가 szSkillName을 배운다.
	// 스킬을 생성하고 초기화 한 다음 pcsCharacter에 추가한다.

	AgpdSkillTemplate	*pcsSkillTemplate	= pThis->m_pagpmSkill->GetSkillTemplate(szSkillName);
	if (!pcsSkillTemplate)
		return FALSE;

	AgpdSkill	*pcsSkill	= NULL;

	switch (pcsBase->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			pcsSkill		= pThis->CreateSkill((AgpdCharacter *) pcsBase, pcsSkillTemplate->m_lID);
		}
		break;
	}

	if (!pcsSkill)
		return FALSE;

//	pcsSkill->m_lMasteryIndex		= lMasteryIndex;

	if (pcsSkillTemplate->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
	{
		if (pThis->m_pagpmProduct)
		{
			// 생산 스킬 템플릿을 가져와라..
			INT32 lLevel = 0;
			INT32 lExp = lActivedSkillPoint;
			lLevel = pThis->m_pagpmProduct->GetLevelOfExp(pcsSkill, lActivedSkillPoint);
			
			pThis->m_pagpmFactors->SetValue(&pcsSkill->m_csFactor, lLevel, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_LEVEL);
			pThis->m_pagpmFactors->SetValue(&pcsSkill->m_csFactor, lLevel, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_POINT);		
			pThis->m_pagpmFactors->SetValue(&pcsSkill->m_csFactor, lActivedSkillPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_EXP);
		}
	}
	else
	{
		pcsSkill->m_lActivedSkillPoint	= lActivedSkillPoint;
		pThis->m_pagpmFactors->SetValue(&pcsSkill->m_csFactor, lActivedSkillPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_LEVEL);
		pThis->m_pagpmFactors->SetValue(&pcsSkill->m_csFactor, lActivedSkillPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_POINT);
	}

	if (!pThis->m_pagpmSkill->InitSkill(pcsSkill))
		return FALSE;

	((PVOID *) pCustData)[1] = (PVOID) pcsSkill;

	return TRUE;
}

BOOL AgsmSkillManager::CBForgetSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSkillManager	*pThis			= (AgsmSkillManager *)	pClass;
	ApBase				*pcsBase		= (ApBase *)			pData;
	AgpdSkill			*pcsSkill		= (AgpdSkill *)			pCustData;

	return pThis->RemoveSkill(pcsSkill);
}

/*
BOOL AgsmSkillManager::GetSelectResult2(COLEDB *pcOLEDB, AgpdCharacter *pcsCharacter)
{
	if (!pcOLEDB ||
		!pcsCharacter)
		return FALSE;

	CHAR	szOwnerName[AGSMDB_MAX_NAME];
	INT32	lMastery							= 0;
	INT32	lTotalSP							= 0;
	CHAR	szSkillTree[129];
	UINT32	ulInstSpec							= 0;
	UINT32	ulInstSpecTime						= 0;
	UINT32	ulConstSpec							= 0;

	ZeroMemory(szOwnerName, sizeof(CHAR) * AGSMDB_MAX_NAME);
	ZeroMemory(szSkillTree, sizeof(CHAR) * 129);

	INT16	nIndex								= 0;
	CHAR	*szBuffer							= NULL;

	INT32	lTotalInputSP						= 0;

	m_pagpmSkill->CBInitCharacter((PVOID) pcsCharacter, (PVOID) m_pagpmSkill, NULL);

	do {

		lMastery								= 0;
		lTotalSP								= 0;
		ulInstSpec								= 0;
		ulInstSpecTime							= 0;
		ulConstSpec								= 0;

		ZeroMemory(szOwnerName, sizeof(CHAR) * AGSMDB_MAX_NAME);
		ZeroMemory(szSkillTree, sizeof(CHAR) * 129);

		nIndex									= 0;
		szBuffer								= NULL;

		if ((szBuffer = pcOLEDB->GetQueryResult(nIndex++)) != NULL)
			strncpy(szOwnerName, szBuffer, AGSMDB_MAX_NAME);

		if ((szBuffer = pcOLEDB->GetQueryResult(nIndex++)) != NULL)
			lMastery				= atoi(szBuffer);
		if ((szBuffer = pcOLEDB->GetQueryResult(nIndex++)) != NULL)
			lTotalSP				= atoi(szBuffer);

		if ((szBuffer = pcOLEDB->GetQueryResult(nIndex++)) != NULL)
			strncpy(szSkillTree, szBuffer, 129);

		if ((szBuffer = pcOLEDB->GetQueryResult(nIndex++)) != NULL)
			ulInstSpec				= (UINT32) atoi(szBuffer);
		if ((szBuffer = pcOLEDB->GetQueryResult(nIndex++)) != NULL)
			ulInstSpecTime			= (UINT32) _atoi64(szBuffer);
		if ((szBuffer = pcOLEDB->GetQueryResult(nIndex++)) != NULL)
			ulConstSpec				= (UINT32) atoi(szBuffer);

		if (!m_pagsmSkill->ParseStringMasteryActiveNodeSequence((ApBase *) pcsCharacter, lMastery, szSkillTree, lTotalSP))
			return FALSE;

		lTotalInputSP	+= lTotalSP;

	} while (pcOLEDB->GetNextRow());

	//m_pagpmSkill->AdjustSkillPoint(pcsCharacter);
	m_pagpmCharacter->UpdateSkillPoint(pcsCharacter, -lTotalInputSP);

	return TRUE;
}
*/


BOOL AgsmSkillManager::GetSelectResult5(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter)
{
	if (!pDatabase ||
		!pcsCharacter)
		return FALSE;

	CHAR	szOwnerName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	INT32	lMastery							= 0;
	INT32	lTotalSP							= 0;

	INT16	nIndex								= 0;
	CHAR	*szBuffer							= NULL;

	//INT32	lTotalInputSP						= 0;

	//m_pagpmSkill->CBInitCharacter((PVOID) pcsCharacter, (PVOID) m_pagpmSkill, NULL);

	AgsdSkillADBase	*pcsSkillADBase	= m_pagsmSkill->GetADBase((ApBase *) pcsCharacter);
	pcsSkillADBase->m_pstMasteryInfo			= new stMasteryInfo;
	if (!pcsSkillADBase->m_pstMasteryInfo)
		return FALSE;

	ZeroMemory(szOwnerName, sizeof(CHAR) * (AGPDCHARACTER_MAX_ID_LENGTH + 1));

	ZeroMemory(pcsSkillADBase->m_pstMasteryInfo->szSkillTree, sizeof(pcsSkillADBase->m_pstMasteryInfo->szSkillTree));
	ZeroMemory(pcsSkillADBase->m_pstMasteryInfo->szSpecialize, sizeof(pcsSkillADBase->m_pstMasteryInfo->szSpecialize));
	ZeroMemory(pcsSkillADBase->m_pstMasteryInfo->szProductCompose, sizeof(pcsSkillADBase->m_pstMasteryInfo->szProductCompose));

	nIndex									= 0;
	szBuffer								= NULL;

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(szOwnerName, szBuffer, AGPDCHARACTER_MAX_ID_LENGTH);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(pcsSkillADBase->m_pstMasteryInfo->szSkillTree, szBuffer, AGSMSKILL_MAX_SKILLTREE_LENGTH);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(pcsSkillADBase->m_pstMasteryInfo->szProductCompose, szBuffer, AGSMSKILL_MAX_SKILLTREE_LENGTH);

	//AuLogFile("Skill.log", "%s\n%s\n%s\n============\n", pcsSkillADBase->m_pstMasteryInfo->szSkillTree, pcsSkillADBase->m_pstMasteryInfo->szProductCompose);

	//m_pagpmCharacter->UpdateSkillPoint(pcsCharacter, -lTotalInputSP);

	return TRUE;
}
