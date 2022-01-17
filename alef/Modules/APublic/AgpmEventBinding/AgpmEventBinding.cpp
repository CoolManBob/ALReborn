//	AgpmEventBinding.cpp
//////////////////////////////////////////////////////////////////////

#include "AgpmEventBinding.h"
#include "ApMemoryTracker.h"
#include <math.h>
#include "AgpmBattleGround.h"

AgpmEventBinding::AgpmEventBinding()
{
	SetModuleName("AgpmEventBinding");

	SetModuleData(sizeof(AgpdBinding));
}

AgpmEventBinding::~AgpmEventBinding()
{
}

BOOL AgpmEventBinding::OnAddModule()
{
	m_pcsAgpmFactors				= (AgpmFactors *)				GetModule("AgpmFactors");
	m_pcsApmMap						= (ApmMap *)					GetModule("ApmMap");
	m_pcsAgpmCharacter				= (AgpmCharacter *)				GetModule("AgpmCharacter");
	m_pcsApmEventManager			= (ApmEventManager *)			GetModule("ApmEventManager");

	if (!m_pcsAgpmFactors ||
		!m_pcsApmMap ||
		!m_pcsAgpmCharacter ||
		!m_pcsApmEventManager)
		return FALSE;

	// Event Manager에 Event를 등록한다.
	if (!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_BINDING, CBEventConstructor, CBEventDestructor, NULL, CBStreamWriteEvent, CBStreamReadEvent, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventBinding::OnInit()
{
	m_pcsAgpmBattleGround			= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	
	//if(!m_pcsAgpmBattleGround)
	//	return FALSE;
		
	if (!m_csAdminBinding.InitializeObject(sizeof(AgpdBinding *), m_csAdminBinding.GetCount()))
		return FALSE;

	m_csGenerateID.Initialize();

	return TRUE;
}

BOOL AgpmEventBinding::OnDestroy()
{
	INT32			lIndex = 0;
	AgpdBinding		**ppcsBinding = (AgpdBinding **) m_csAdminBinding.GetObjectSequence(&lIndex);
	while (ppcsBinding && *ppcsBinding)
	{
		RemoveBinding((*ppcsBinding)->m_lID);

		ppcsBinding = (AgpdBinding **) m_csAdminBinding.GetObjectSequence(&lIndex);
	}

	m_csAdminBinding.RemoveObjectAll();

	return TRUE;
}

BOOL AgpmEventBinding::CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventBinding		*pThis				= (AgpmEventBinding *)	pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)			pData;

//	pcsEvent->m_pvData		= (PVOID) malloc(sizeof(AgpdBindingAttach));
	pcsEvent->m_pvData		= (PVOID) new BYTE[sizeof(AgpdBindingAttach)];

	ZeroMemory(pcsEvent->m_pvData, sizeof(AgpdBindingAttach));

	return TRUE;
}

BOOL AgpmEventBinding::CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventBinding		*pThis				= (AgpmEventBinding *)	pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)			pData;

	AgpdBindingAttach		*pcsAttach			= (AgpdBindingAttach *)	pcsEvent->m_pvData;
	if (pcsAttach)
	{
		AgpdBinding			*pcsBinding			= pThis->GetBinding(pcsAttach->m_lBindingID);
		if (pcsBinding)
			pThis->RemoveBinding(pcsBinding->m_lID);

//		free(pcsAttach);
		delete [] (BYTE*)pcsAttach;
	}

	return TRUE;
}

BOOL AgpmEventBinding::SetMaxBindingData(INT32 lCount)
{
	return m_csAdminBinding.SetCount(lCount);
}

AgpdBinding* AgpmEventBinding::CreateBindingData()
{
	AgpdBinding	*pcsBinding	= (AgpdBinding *) CreateModuleData();
	if (pcsBinding)
	{
		pcsBinding->m_lID					= 0;

		pcsBinding->m_Mutex.Init((PVOID) pcsBinding);

		ZeroMemory(pcsBinding->m_szBindingName, sizeof(CHAR) * (AGPDBINDING_MAX_NAME + 1));
		ZeroMemory(pcsBinding->m_szTownName, sizeof(CHAR) * (AGPDBINDING_MAX_TOWN_NAME + 1));

		ZeroMemory(&pcsBinding->m_stBasePos, sizeof(AuPOS));

		pcsBinding->m_ulRadius				= 0;
		pcsBinding->m_eBindingType			= AGPDBINDING_TYPE_NONE;
	}

	return pcsBinding;
}

BOOL AgpmEventBinding::DestroyBindingData(AgpdBinding *pcsBinding)
{
	if (!pcsBinding)
		return FALSE;

	pcsBinding->m_Mutex.Destroy();

	return DestroyModuleData(pcsBinding);
}

AgpdBinding* AgpmEventBinding::AddBinding(CHAR *szBindingName, CHAR *szTownName, AuPOS *pstBasePos, UINT32 ulRadius, AgpdBindingType eBindingType)
{
	if (!szBindingName || !szBindingName[0] || !pstBasePos)
		return NULL;

	AgpdBinding	*pcsBinding	= CreateBindingData();
	if (!pcsBinding)
		return NULL;

	pcsBinding->m_lID				= m_csGenerateID.GetID();

	ZeroMemory(pcsBinding->m_szBindingName, sizeof(CHAR) * (AGPDBINDING_MAX_NAME + 1));
	strncpy(pcsBinding->m_szBindingName, szBindingName, AGPDBINDING_MAX_NAME);
	if (szTownName && szTownName[0])
	{
		ZeroMemory(pcsBinding->m_szTownName, sizeof(CHAR) * (AGPDBINDING_MAX_TOWN_NAME + 1));
		strncpy(pcsBinding->m_szTownName, szTownName, AGPDBINDING_MAX_TOWN_NAME);
	}

	pcsBinding->m_stBasePos			= *pstBasePos;

	pcsBinding->m_ulRadius			= ulRadius;

	pcsBinding->m_eBindingType		= eBindingType;

	if (!m_csAdminBinding.AddBinding(pcsBinding, pcsBinding->m_lID, pcsBinding->m_szBindingName))
	{
		DestroyBindingData(pcsBinding);
		return NULL;
	}

	return pcsBinding;
}

BOOL AgpmEventBinding::RemoveBinding(INT32 lID)
{
	AgpdBinding	*pcsBinding	= GetBinding(lID);
	if (!pcsBinding)
		return FALSE;

	m_csAdminBinding.RemoveBinding(pcsBinding->m_lID, pcsBinding->m_szBindingName);

	return DestroyBindingData(pcsBinding);
}

BOOL AgpmEventBinding::RemoveBinding(CHAR *szBindingName)
{
	AgpdBinding	*pcsBinding	= GetBinding(szBindingName);
	if (!pcsBinding)
		return FALSE;

	m_csAdminBinding.RemoveBinding(pcsBinding->m_lID, pcsBinding->m_szBindingName);

	return DestroyBindingData(pcsBinding);
}

AgpdBinding* AgpmEventBinding::GetBinding(INT32 lID)
{
	return m_csAdminBinding.GetBinding(lID);
}

AgpdBinding* AgpmEventBinding::GetBinding(CHAR *szBindingName)
{
	return m_csAdminBinding.GetBinding(szBindingName);
}

AgpdBinding* AgpmEventBinding::GetBindingByTown(CHAR *szTownName, AgpdBindingType eBindingType)
{
	INT32		lIndex	= 0;
	AgpdBinding	**ppcsBinding	= (AgpdBinding **) m_csAdminBinding.GetObjectSequence(&lIndex);
	while (ppcsBinding && *ppcsBinding)
	{
		if (strncmp((*ppcsBinding)->m_szTownName, szTownName, AGPDBINDING_MAX_TOWN_NAME) == 0 &&
			(*ppcsBinding)->m_eBindingType == eBindingType)
		{
			return *ppcsBinding;
		}

		ppcsBinding	= (AgpdBinding **) m_csAdminBinding.GetObjectSequence(&lIndex);
	}

	return NULL;
}

INT32 AgpmEventBinding::GetBindingsByTown(CHAR *szTownName, AgpdBindingType eBindingType, AgpdBinding * apBindingList[])
{
	INT32		lIndex	= 0;
	INT32		lCount	= 0;
	AgpdBinding	**ppcsBinding	= (AgpdBinding **) m_csAdminBinding.GetObjectSequence(&lIndex);
	while (ppcsBinding && *ppcsBinding)
	{
		if (strncmp((*ppcsBinding)->m_szTownName, szTownName, AGPDBINDING_MAX_TOWN_NAME) == 0 &&
			(*ppcsBinding)->m_eBindingType == eBindingType)
		{
			apBindingList[lCount++] = *ppcsBinding;
		}

		ppcsBinding	= (AgpdBinding **) m_csAdminBinding.GetObjectSequence(&lIndex);
	}

	return lCount;
}

INT32 AgpmEventBinding::GetLength(AuPOS *pstPos1, AuPOS *pstPos2)
{
	FLOAT				fx = pstPos1->x - pstPos2->x;
	FLOAT				fz = pstPos1->z - pstPos2->z;

	return (INT32) sqrt(fx * fx + fz * fz);
}

INT32 AgpmEventBinding::GetBindingsByTown2(CHAR *szTownName, AgpdBindingType eBindingType, AgpdBinding * apBindingList[], BOOL bInner, AuPOS *pstBasePos)
{
	if (!pstBasePos || !apBindingList || !szTownName || !szTownName[0])
		return 0;

	AgpdBinding*	apTempBindingList[20];

	ZeroMemory(apTempBindingList, sizeof(apTempBindingList));

	// 2007.01.16. steeple
	// Inner 체크 새로 한다.
	if(eBindingType == AGPDBINDING_TYPE_SIEGEWAR_DEFENSE_INNER || eBindingType == AGPDBINDING_TYPE_SIEGEWAR_DEFENSE_OUTTER)
	{
		eBindingType = bInner ? AGPDBINDING_TYPE_SIEGEWAR_DEFENSE_INNER : AGPDBINDING_TYPE_SIEGEWAR_DEFENSE_OUTTER;
	}

	INT32		lIndex	= 0;
	INT32		lCount	= 0;
	AgpdBinding	**ppcsBinding	= (AgpdBinding **) m_csAdminBinding.GetObjectSequence(&lIndex);
	while (ppcsBinding && *ppcsBinding)
	{
		if (strncmp((*ppcsBinding)->m_szTownName, szTownName, AGPDBINDING_MAX_TOWN_NAME) == 0 &&
			(*ppcsBinding)->m_eBindingType == eBindingType)
		{
			apTempBindingList[lCount++] = *ppcsBinding;
		}

		ppcsBinding	= (AgpdBinding **) m_csAdminBinding.GetObjectSequence(&lIndex);
	}

	if (lCount > 0)
	{
		if (bInner)
		{
			INT32	lMaxLength	= 0;
			INT32	lMaxIndex	= 0;

			for (int i = 0; i < lCount; ++i)
			{
				INT32	lLength	= GetLength(pstBasePos, &apTempBindingList[i]->m_stBasePos);

				if (lLength > lMaxLength)
				{
					lMaxLength	= lLength;
					lMaxIndex	= i;
				}
			}

			INT32	lTotal	= 0;
			for (int i = 0; i < lCount; ++i)
			{
				if (i != lMaxIndex)
					apBindingList[lTotal++]	= apTempBindingList[i];
			}

			return lTotal;
		}
		else
		{
			INT32	lMaxLength	= 0;
			INT32	lMaxIndex	= 0;

			for (int i = 0; i < lCount; ++i)
			{
				INT32	lLength	= GetLength(pstBasePos, &apTempBindingList[i]->m_stBasePos);

				if (lLength > lMaxLength)
				{
					lMaxLength	= lLength;
					lMaxIndex	= i;
				}
			}

			apBindingList[0]	= apTempBindingList[lMaxIndex];

			return 1;

		}
	}

	return lCount;
}

AgpdBinding* AgpmEventBinding::GetBindingForNewCharacter(INT32 lRace, INT32 lClass)
{
	INT32		lIndex	= 0;
	AgpdBinding	**ppcsBinding	= (AgpdBinding **) m_csAdminBinding.GetObjectSequence(&lIndex);
	while (ppcsBinding && *ppcsBinding)
	{
//		if ((*ppcsBinding)->m_eBindingType == AGPDBINDING_TYPE_NEW_CHARACTER &&
//			(*ppcsBinding)->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_RACE] == lRace &&
//			(*ppcsBinding)->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_CLASS] == lClass)

		if ((*ppcsBinding)->m_eBindingType == AGPDBINDING_TYPE_NEW_CHARACTER &&
			(*ppcsBinding)->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_RACE] == lRace)
		{
			return *ppcsBinding;
		}

		ppcsBinding	= (AgpdBinding **) m_csAdminBinding.GetObjectSequence(&lIndex);
	}

	return NULL;
}

BOOL AgpmEventBinding::GetBindingPositionForResurrection(AgpdCharacter *pcsCharacter, AuPOS *pstDestPos)
{
	if (!pcsCharacter || !pstDestPos)
		return FALSE;
		
	INT16 nLastExistBindingIndex = pcsCharacter->m_nLastExistBindingIndex;
	INT16 nCurrentExistBindingIndex = pcsCharacter->m_nBindingRegionIndex;
	
	if( m_pcsAgpmBattleGround && m_pcsAgpmBattleGround->IsBattleGround(nLastExistBindingIndex))
	{
		INT32 lRace = m_pcsAgpmFactors->GetRace(&pcsCharacter->m_csFactor);
		switch(lRace)
		{
			case AURACE_TYPE_ORC:			nLastExistBindingIndex = REGIONINDEX_BATTLEGROUND_ORC_CAMP;			break;
			case AURACE_TYPE_HUMAN:			nLastExistBindingIndex = REGIONINDEX_BATTLEGROUND_HUMAN_CAMP;		break;
			case AURACE_TYPE_MOONELF:		nLastExistBindingIndex = REGIONINDEX_BATTLEGROUND_MOONELF_CAMP;		break;
			case AURACE_TYPE_DRAGONSCION:	nLastExistBindingIndex = REGIONINDEX_BATTLEGROUND_DRAGONSCION_CAMP;	break;
		}
	}

	ApmMap::RegionTemplate	*pstRegionTemplate	= m_pcsApmMap->GetTemplate(nLastExistBindingIndex);
	if (!pstRegionTemplate || !pstRegionTemplate->pStrName || !pstRegionTemplate->pStrName[0])
		return FALSE;

	// 현재 Region Template에 Resurrection Point가 지정되어있다면 그 지점을 Resurrection 위치로 만들어준다.
	ApmMap::RegionTemplate *pstCurrentRegionTemplate = m_pcsApmMap->GetTemplate(nCurrentExistBindingIndex);
	if(!pstCurrentRegionTemplate || !pstCurrentRegionTemplate->pStrName || !pstRegionTemplate->pStrName[0])
		return FALSE;

	if(pstCurrentRegionTemplate->stResurrectionPos.x != 0.0f || pstCurrentRegionTemplate->stResurrectionPos.z != 0.0f)
	{
		*pstDestPos = pstCurrentRegionTemplate->stResurrectionPos;

		return TRUE;
	}

	AgpdBinding	*pcsBinding	= GetBindingByTown(pstRegionTemplate->pStrName, AGPDBINDING_TYPE_RESURRECTION);

	if (pcsBinding)
	{
		if (!m_pcsApmEventManager->GetRandomPos(&pcsBinding->m_stBasePos, pstDestPos, 0, (FLOAT)pcsBinding->m_ulRadius, TRUE))
			*pstDestPos	= pcsBinding->m_stBasePos;

		return TRUE;
	}
	else
	{
		return GetBindingPositionForNewCharacter(pcsCharacter, pstDestPos);
	}

	return FALSE;
}

BOOL AgpmEventBinding::GetBindingPositionForNewCharacter(AgpdCharacter *pcsCharacter, AuPOS *pstDestPos)
{
	if (!pcsCharacter || !pstDestPos)
		return FALSE;

	AgpdBinding	*pcsBinding	= GetBindingForNewCharacter(m_pcsAgpmFactors->GetRace(&pcsCharacter->m_csFactor), m_pcsAgpmFactors->GetClass(&pcsCharacter->m_csFactor));
	if (!pcsBinding)
		return FALSE;

	if (!m_pcsApmEventManager->GetRandomPos(&pcsBinding->m_stBasePos, pstDestPos, 0, (FLOAT)pcsBinding->m_ulRadius, TRUE))
		*pstDestPos	= pcsBinding->m_stBasePos;

	return TRUE;
}

BOOL AgpmEventBinding::GetBindingPositionForSiegeWarResurrection(AgpdCharacter *pcsCharacter, AuPOS *pstDestPos, CHAR *szCastle, BOOL bOffense, BOOL bInner, INT32 lIndex, BOOL bCheckBlocking)
{
	if (!pcsCharacter || !pstDestPos || !szCastle)
		return FALSE;

	INT32			lTotal	= 0;
	AgpdBinding*	apBindingList[20];
	
	ZeroMemory(apBindingList, sizeof(apBindingList));

	if (bOffense)
		lTotal = GetBindingsByTown(szCastle, AGPDBINDING_TYPE_SIEGEWAR_OFFENSE, apBindingList);
	else
	{
		lTotal = GetBindingsByTown2(szCastle, AGPDBINDING_TYPE_SIEGEWAR_DEFENSE_INNER, apBindingList, bInner, &pcsCharacter->m_stPos);
	}
	
	if (lIndex >= 0 && lIndex < lTotal)
	{
		AgpdBinding *pcsBinding = apBindingList[lIndex];
		if (pcsBinding && m_pcsApmEventManager->GetRandomPos(&pcsBinding->m_stBasePos, pstDestPos, 0, (FLOAT)pcsBinding->m_ulRadius, bCheckBlocking))
		{
			// 2007.01.16. steeple
			// 위에 GetRandomPos 함수에서 세팅해준다. 고로 여기선 주석.
			//*pstDestPos	= pcsBinding->m_stBasePos;
			return TRUE;
		}
	}
	else if (lTotal > 0)
	{
		// random
		INT32 i = rand() % lTotal;
		AgpdBinding *pcsBinding = apBindingList[i];
		if (pcsBinding && m_pcsApmEventManager->GetRandomPos(&pcsBinding->m_stBasePos, pstDestPos, 0, (FLOAT)pcsBinding->m_ulRadius, TRUE))
		{
			// 2007.01.16. steeple
			// 위에 GetRandomPos 함수에서 세팅해준다. 고로 여기선 주석.
			//*pstDestPos	= pcsBinding->m_stBasePos;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgpmEventBinding::GetBindingForArchlord(AuPOS *pstDestPos, CHAR *szCastle)
{
	if (!pstDestPos || !szCastle)
		return FALSE;

	AgpdBinding	*pcsBinding	= GetBindingByTown(szCastle, AGPDBINDING_TYPE_SIEGEWAR_ARCHLORD);
	if (!pcsBinding)
		return FALSE;

	if (!m_pcsApmEventManager->GetRandomPos(&pcsBinding->m_stBasePos, pstDestPos, 0, (FLOAT) pcsBinding->m_ulRadius, TRUE))
		*pstDestPos	= pcsBinding->m_stBasePos;

	return TRUE;
}

BOOL AgpmEventBinding::GetBindingForArchlordAttacker(AuPOS *pstDestPos, CHAR *szCastle)
{
	if (!pstDestPos || !szCastle)
		return FALSE;

	AgpdBinding	*pcsBinding	= GetBindingByTown(szCastle, AGPDBINDING_TYPE_SIEGEWAR_ARCHLORD_ATTACKER);
	if (!pcsBinding)
		return FALSE;

	if (!m_pcsApmEventManager->GetRandomPos(&pcsBinding->m_stBasePos, pstDestPos, 0, (FLOAT) pcsBinding->m_ulRadius, TRUE))
		*pstDestPos	= pcsBinding->m_stBasePos;

	return TRUE;
}