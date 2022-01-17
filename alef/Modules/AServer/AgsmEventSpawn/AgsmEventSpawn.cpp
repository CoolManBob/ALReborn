// AgsmEventSpawn.cpp: implementation of the AgsmEventSpawn class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmEventSpawn.h"
#include "AgsmBattleGround.h"

INT32	g_lActiveCellCount		= 0;
INT32	g_lActiveSpawnCharCount	= 0;
BOOL	g_bPrintActiveCellCount	= 0;

const	INT32 SPAWN_GROUP_DIVISION_COUNT	= 5;

extern LONG	g_lNumCharacter;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmEventSpawn::AgsmEventSpawn()
{
	SetModuleName("AgsmEventSpawn");

	//EnableIdle2(TRUE);
	EnableIdle3(TRUE);

	m_lLastSpawnGroupIndex = 0;
	
	m_pcsAgsmBattleGround	= NULL;
}

AgsmEventSpawn::~AgsmEventSpawn()
{
}

BOOL	AgsmEventSpawn::OnAddModule()
{
	// 상위 Module들 가져온다.
	m_pcsApmEventManager	= (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgpmCharacter		= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmEventSpawn		= (AgpmEventSpawn *) GetModule("AgpmEventSpawn");
	m_pcsAgpmItem			= (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmAI2			= (AgpmAI2 *) GetModule("AgpmAI2");
	m_pcsAgpmDropItem2		= (AgpmDropItem2 *) GetModule("AgpmDropItem2");
	m_pcsAgsmAOIFilter		= (AgsmAOIFilter *) GetModule("AgsmAOIFilter");
	m_pcsAgsmCharacter		= (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pcsAgsmCharManager	= (AgsmCharManager *) GetModule("AgsmCharManager");
	m_pcsAgsmItemManager	= (AgsmItemManager *) GetModule("AgsmItemManager");
	m_pcsAgsmDeath			= (AgsmDeath *) GetModule("AgsmDeath");
	m_pcsAgsmUsedCharDataPool	= (AgsmUsedCharDataPool *) GetModule("AgsmUsedCharDataPool");
	m_pcsAgsmFactors		= (AgsmFactors *) GetModule("AgsmFactors");
	m_pcsAgpmConfig			= (AgpmConfig *) GetModule("AgpmConfig");
	m_pcsAgpmSiegeWar		= (AgpmSiegeWar *) GetModule("AgpmSiegeWar");

	if (!m_pcsApmEventManager ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmEventSpawn ||
		!m_pcsAgpmDropItem2 ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgsmCharManager ||
		!m_pcsAgpmItem ||
		!m_pcsAgsmAOIFilter ||
		!m_pcsAgsmItemManager ||
		!m_pcsAgpmAI2 ||
		!m_pcsAgsmDeath ||
		!m_pcsAgsmUsedCharDataPool ||
		!m_pcsAgsmFactors ||
		!m_pcsAgpmConfig ||
		!m_pcsAgpmSiegeWar )
		return FALSE;

	if(!m_pcsAgpmEventSpawn->SetCallbackSpawn(CBSpawn, this))
		return FALSE;

	return TRUE;
}

BOOL	AgsmEventSpawn::OnInit()
{
	m_pcsAgsmBattleGround	= (AgsmBattleGround*)GetModule("AgsmBattleGround");

	return TRUE;
}

BOOL	AgsmEventSpawn::OnDestroy()
{
	return TRUE;
}

BOOL	AgsmEventSpawn::OnIdle3(UINT32 ulClockCount)
{
	PROFILE("AgsmEventSpawn::OnIdle3");

	//STOPWATCH2(GetModuleName(), _T("OnIdle3"));

	// 2007.03.15. steeple
	// 추가적으로 Spawn 이 필요한 것들
	std::vector<AdditionalSpawnData> vcAdditionalSpawn;
	m_IdleMutex.WLock();
	if(!m_vcAdditionalSpawn.empty())
	{
		vcAdditionalSpawn.assign(m_vcAdditionalSpawn.begin(), m_vcAdditionalSpawn.end());
		m_vcAdditionalSpawn.clear();
	}
	m_IdleMutex.Release();

	if(!vcAdditionalSpawn.empty())
	{
		std::vector<AdditionalSpawnData>::iterator iter = vcAdditionalSpawn.begin();
		while(iter != vcAdditionalSpawn.end())
		{
			AdditionalSpawnData& SpawnData = *iter;
			SpawnData.m_pfCallback(&SpawnData, SpawnData.m_pClass, NULL);

			++iter;
		}
	}

	AgpdSpawnGroup *	pstGroup = NULL;
	INT32				lIndex = 0;
	INT32				lGroupCount = m_pcsAgpmEventSpawn->GetMaxGroupCount();
	INT32				lCountPerOnce = lGroupCount / SPAWN_GROUP_DIVISION_COUNT;

	if(lCountPerOnce == 0)
		return TRUE;

	g_lActiveCellCount		= 0;
	g_lActiveSpawnCharCount	= 0;

	if(m_lLastSpawnGroupIndex != 0)
		lIndex = m_lLastSpawnGroupIndex;

	for (pstGroup = m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex); pstGroup; pstGroup = m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex))
	{
		ProcessSpawnGroup(pstGroup, ulClockCount);

		if(lIndex >= m_lLastSpawnGroupIndex + lCountPerOnce)
		{
			m_lLastSpawnGroupIndex = lIndex;
			break;
		}
	}

	// 혹시 Index 가 넘어갔다면 지난 GroupIndex 를 0 으로 해준다.
	if(/*lIndex >= lGroupCount ||*/ !pstGroup)
		m_lLastSpawnGroupIndex = 0;


	//for (pstGroup = m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex); pstGroup; pstGroup = m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex))
	//{
	//	ProcessSpawnGroup(pstGroup, ulClockCount);
	//}

	if (g_bPrintActiveCellCount)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "Current Active Cell : %d, Spawned Character : %d, NumCharacter : %d \n", g_lActiveCellCount / 3, g_lActiveSpawnCharCount / 3, g_lNumCharacter);
		AuLogFile_s("LOG\\ActiveCellCount.txt", strCharBuff);

		g_bPrintActiveCellCount	= 0;
		g_lActiveCellCount		= 0;
		g_lActiveSpawnCharCount	= 0;
	}

	return TRUE;
}

BOOL	AgsmEventSpawn::ProcessSpawnGroup(AgpdSpawnGroup *pstGroup, UINT32 ulClockCount)
{
	PROFILE("AgsmEventSpawn::ProcessSpawnGroup");

	//STOPWATCH2(GetModuleName(), _T("ProcessSpawnGroup"));

	ApdEvent *			pstEvent;
	INT16				nTry = 0;

	// Interval을 Check해서 처리해야 될지를 결정한다.
	//if (pstGroup->m_ullPrevProcessTime + pstGroup->m_lInterval > ulClockCount)
	//	return TRUE;

	//do
	//{
		for (pstEvent = pstGroup->m_pstChild; pstEvent; pstEvent = ((AgpdSpawn *) pstEvent->m_pvData)->m_pstNext)
		{
			AuMATRIX	*pstDirection	= NULL;;
			AuPOS		stSpawnPos	= *m_pcsApmEventManager->GetBasePos(pstEvent->m_pcsSource, &pstDirection);

			//if (((AgpdSpawn *) pstEvent->m_pvData)->m_lSiegeWarType == AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_NPC_GUILD &&
			//	(!m_pcsAgpmConfig->IsSetServerStatusFlag(AGPM_CONFIG_FLAG_SIEGE_WAR) ||
			//	 m_pcsAgpmSiegeWar->GetCurrentSiegeWarType(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(stSpawnPos)) != AGPD_SIEGE_WAR_TYPE_NPC_GUILD))
			//	continue;

			// 2007.01.18. steeple
			// SiegeWar Object 도 여기서 스폰 안하게끔 벼경했다.
			INT32 lSiegeWarType = ((AgpdSpawn *) pstEvent->m_pvData)->m_lSiegeWarType;
			if(lSiegeWarType == AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_NPC_GUILD ||
				lSiegeWarType == AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_OBJECT ||
				lSiegeWarType == AGPDSPAWN_ARCHLORD_TYPE_SECRET_DUNGEON ||
				lSiegeWarType == AGPDSPAWN_ARCHLORD_TYPE_DEKAIN)
				continue;
				
			// 배틀그라운드 몬스터 설정
			//if((lSiegeWarType == AGPDSPAWN_BATTLEGROUND_NORMAL && m_pcsAgsmBattleGround->GetBattleGroundState() != BATTLEGROUND_STATE_NORMAL)
			//	|| (lSiegeWarType == AGPDSPAWN_BATTLEGROUND_ABILITY && m_pcsAgsmBattleGround->GetBattleGroundState() == BATTLEGROUND_STATE_NORMAL))
			if(lSiegeWarType == AGPDSPAWN_BATTLEGROUND_NORMAL)
			{
				if( m_pcsAgsmBattleGround->GetBattleGroundState() != BATTLEGROUND_STATE_NORMAL)
				continue;
			}
			
			if(lSiegeWarType == AGPDSPAWN_BATTLEGROUND_ABILITY)
			{
				continue;
			}

			ProcessSpawn(pstEvent, ulClockCount);

			if (!pstEvent->m_pvData)
				break;
		}

		//++nTry;
	//} while (pstGroup->m_lCharNum < pstGroup->m_lMinChar && nTry < 3);

	/*
	if (nTry >= 2)
	{
		OutputDebugString("AgsmEventSpawn::ProcessSpawnGroup() Error (1) !!!\n");
	}
	*/

	//pstGroup->m_ullPrevProcessTime = ulClockCount;

	return TRUE;
}

BOOL	AgsmEventSpawn::ProcessSpawn(ApdEvent *pstEvent, UINT32 ulClockCount)
{
	PROFILE("AgsmEventSpawn::ProcessSpawn");

	if (!pstEvent || !pstEvent->m_pvData || pstEvent->m_eFunction != APDEVENT_FUNCTION_SPAWN)
		return FALSE;

	AgpdSpawn *			pstSpawn	= (AgpdSpawn *) pstEvent->m_pvData;
	AgpdSpawnGroup *	pstGroup	= pstSpawn->m_pstParent;

	INT32 lTotalCount = pstSpawn->m_lTotalCharacter;
	if(lTotalCount == 0)
		return TRUE;

	CTime currenttime = CTime::GetCurrentTime();

	for(INT32 i = 0; i < lTotalCount; ++i)
	{
		if(pstSpawn->m_stSpawnCharacters[i].m_lID != 0)
			continue;

		if(pstSpawn->m_stSpawnCharacters[i].m_ulRespawnTime > currenttime)
			continue;

		INT32 lRandom = m_csRandomNumber.randInt(10000);
		AgpdSpawnConfig* pcsSpawnConfig = NULL;

		for(INT32 i2 = 0; i2 < AGPDSPAWN_MAX_CHAR_NUM; ++i2)
		{
			if(pstSpawn->m_stSpawnConfig[i2].m_lSpawnRate < lRandom)
			{
				lRandom -= pstSpawn->m_stSpawnConfig[i2].m_lSpawnRate;
			}
			else
			{
				pcsSpawnConfig = &pstSpawn->m_stSpawnConfig[i2];
				break;
			}
		}

		if (!pcsSpawnConfig)
		{
			OutputDebugString("AgsmEventSpawn::ProcessSpawn() Error (1) !!!\n");
			pstSpawn->m_stSpawnCharacters[i].m_ulRespawnTime = currenttime + CTimeSpan(0, 0, 0, pstSpawn->m_lSpawnInterval);
			continue;
		}

		AgpdCharacter* pcsCharacter = SpawnCharacter(pstEvent, pcsSpawnConfig->m_lAITID, pcsSpawnConfig->m_lTID, &pstSpawn->m_stEvent);
		if(pcsCharacter)
		{
			pstSpawn->m_stSpawnCharacters[i].m_lID = pcsCharacter->m_lID;
			pstSpawn->m_stSpawnCharacters[i].m_ulRespawnTime = 0;

			pstSpawn->m_Mutex.WLock();
			pstSpawn->m_Mutex.Release();

			g_lActiveSpawnCharCount++;
		}
		else
		{
			/*CHAR strBuff[256] = { 0, };
			sprintf_s(strBuff, sizeof(strBuff), "SpawnCharacter() Error - [%s] [%d]\n", pstSpawn->m_szName, pcsSpawnConfig->m_lTID);
			OutputDebugString(strBuff);*/

			pstSpawn->m_stSpawnCharacters[i].m_ulRespawnTime = currenttime + CTimeSpan(0, 0, 0, pstSpawn->m_lSpawnInterval);
		}
	}

	return TRUE;
}

AgpdCharacter* AgsmEventSpawn::SpawnCharacter(ApdEvent *pstEvent, INT32 lAI2ID, INT32 lCTID, ApdEventAttachData *pstEvents)
{
	PROFILE("AgsmEventSpawn::SpawnCharacter");

	if (!pstEvent || !pstEvent->m_pvData || pstEvent->m_eFunction != APDEVENT_FUNCTION_SPAWN)
		return NULL;

	//STOPWATCH2(GetModuleName(), _T("SpawnCharacter"));

	AgpdSpawn			*pstSpawn	= (AgpdSpawn *) pstEvent->m_pvData;
	AuPOS				stSpawnPos;
	AgpdCharacter		*pcsCharacter;
	AgsdCharacter		*pcsAgsdCharacter;
	AgpdAI2Template		*pcsAI2Template;
	
	//INT32				lIndex;

	pcsAI2Template = m_pcsAgpmAI2->m_aAI2Template.GetAITemplate( lAI2ID );

	if( pcsAI2Template )
	{
		AgpdCharacterTemplate	*pcsCharacterTemplate	= m_pcsAgpmCharacter->GetCharacterTemplate(lCTID);
		if ((pcsCharacterTemplate &&
			pcsCharacterTemplate->m_ulCharType & AGPMCHAR_TYPE_TRAP) ||
			pcsAI2Template->m_eNPCAI2Type == AGPMAI2_TYPE_FIXED_NPC )
		{
			AuMATRIX			*pstDirection;

			stSpawnPos = *m_pcsApmEventManager->GetBasePos( pstEvent->m_pcsSource, &pstDirection );
		}
		else
		{
			if (!m_pcsApmEventManager->GetRandomPos(pstEvent, &stSpawnPos))
			{
				//OutputDebugString("AgsmEventSpawn::SpawnCharacter() Error (1) !!!\n");
				return NULL;
			}
		}
	}
	else
	{
		return NULL;
	}

	INT32 lSpawnCharacterCID = 0;

	{
		pcsCharacter = m_pcsAgsmCharManager->CreateCharacter(NULL, NULL, lCTID, 0, FALSE);
		if (!pcsCharacter)
			return NULL;

		// Lock 해준다. 2006.05.04. steeple
		if (!pcsCharacter->m_Mutex.WLock())
		{
			m_pcsAgpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
			return NULL;
		}

		lSpawnCharacterCID = pcsCharacter->m_lID;
		pcsCharacter->m_stPos = stSpawnPos;
		
		AgpdSpawnADChar* pcsSpawnADChar = m_pcsAgpmEventSpawn->GetCharacterData(pcsCharacter);
		if( pcsSpawnADChar )
		{
			pcsSpawnADChar->m_pstEvent = pstEvent;
			pcsSpawnADChar->m_pstSpawn = pstSpawn;
		}

		if (pstEvent->m_pcsSource && pstEvent->m_pcsSource->m_eType == APBASE_TYPE_OBJECT)
		{
			ApdObject *	pcsObject = (ApdObject *) pstEvent->m_pcsSource;
			m_pcsAgpmCharacter->TurnCharacter(pcsCharacter, pcsObject->m_fDegreeX, pcsObject->m_fDegreeY);
		}

		// 이벤트 flag 세팅
		SetGiftBoxStatus(pcsCharacter);

		// AI의 Attribute에 고정형 몬스터라고 설정된 내용을 적용
		if (TRUE == pcsAI2Template->m_bDontMove)
		{
			pcsCharacter->m_ulSpecialStatus |= AGPDCHAR_SPECIAL_STATUS_DONT_MOVE;
		}

		m_pcsAgpmCharacter->UpdateInit(pcsCharacter);

		if (!m_pcsAgsmCharManager->EnterGameWorld(pcsCharacter, FALSE))
		{
			pcsCharacter->m_Mutex.Release();

			m_pcsAgpmCharacter->RemoveCharacter(pcsCharacter->m_lID);

			return NULL;
		}

		if (m_pcsAgsmCharacter)
		{
			pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
			pcsAgsdCharacter->m_bDestroyWhenDie = TRUE;
		}

		if (!m_pcsApmEventManager->InitObject(pcsCharacter, pstEvents))
		{
			ASSERT(!"AgsmEventSpawn::SpawnCharacter() EventManager InitObject() Error !!!");
		}

		VERIFY(EnumCallback(AGSMSPAWN_CB_SPAWN, pcsCharacter, (void *)&lAI2ID));

		pcsCharacter->m_Mutex.Release();
	}

	return pcsCharacter;
}

// 2006.09.28. steeple
// 이곳 저곳에 있던 함수 이리로 옮겨왔다.
BOOL AgsmEventSpawn::SpawnCharacter(INT32 lTID, INT32 lAI2ID, AuPOS stPos, AgpdSpawn* pstSpawn, AgpdCharacter* pcsBoss, BOOL bAdmin)
{
	/*
	AgpdCharacterTemplate* pcsTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(lTID);
	if(!pcsTemplate)
		return FALSE;

	// 캐릭터를 만든다.
	AgpdCharacter* pcsCharacter = m_pcsAgsmCharManager->CreateCharacter(NULL, NULL, pcsTemplate->m_lID, 0, FALSE);
	if(!pcsCharacter)
		return FALSE;

	pcsCharacter->m_Mutex.WLock();
	pcsCharacter->m_stPos = stPos;
	
	if(bAdmin)
		m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_ADMIN_SPAWNED_CHAR);

	m_pcsAgpmCharacter->UpdateInit(pcsCharacter);

	if(!m_pcsAgsmCharManager->EnterGameWorld(pcsCharacter, FALSE))
	{
		pcsCharacter->m_Mutex.Release();
		m_pcsAgpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
		return FALSE;
	}

	AgsdCharacter* pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
	if(pcsAgsdCharacter)
		pcsAgsdCharacter->m_bDestroyWhenDie = TRUE;

	if(pstSpawn)
	{
		ApdEventAttachData* pstEvents = &pstSpawn->m_stEvent;

		if(pstEvents)
		{
			if(!m_pcsApmEventManager->InitObject(pcsCharacter, pstEvents))
			{
				ASSERT(!"AgsmEventSpawn::SpawnCharacter() EventManager InitObject() Error !!!");
			}
		}
	}

	EnumCallback(AGSMSPAWN_CB_SPAWN, pcsCharacter, &lAI2ID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
	*/
	
	AgpdCharacterTemplate* pcsTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(lTID);
	if(!pcsTemplate)
		return FALSE;

	INT32 lIndex = 0;

	AgpdSpawn* pstSpawnPrime = NULL;
	ApdEvent* pstEvent = NULL;

	AgpdSpawnGroup* pstGroup = NULL;

	BOOL bBreak = FALSE;
	for(pstGroup = m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex); pstGroup && !bBreak; pstGroup = m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex))
	{
		for(pstEvent = pstGroup->m_pstChild; pstEvent && !bBreak; pstEvent = ((AgpdSpawn*)pstEvent->m_pvData)->m_pstNext)
		{
			pstSpawnPrime = (AgpdSpawn*)pstEvent->m_pvData;
			if(!pstSpawnPrime)
				break;

			for(INT32 i = 0; i < AGPDSPAWN_MAX_CHAR_NUM; i++)
			{
				if(pstSpawnPrime->m_stSpawnConfig[i].m_lTID == 0)
					break;

				// 해당 TID 를 찾았다. 바로 리턴
				if(pstSpawnPrime->m_stSpawnConfig[i].m_lTID == lTID)
				{
					bBreak = TRUE;
					break;
				}
			}
			if (bBreak)
				break;
		}
		if (bBreak)
			break;
	}

	INT32 lSpawnCharacterCID = 0;

	{
		// 캐릭터를 만든다.
		AgpdCharacter* pcsCharacter = m_pcsAgsmCharManager->CreateCharacter(NULL, NULL, pcsTemplate->m_lID, 0, FALSE);
		if(!pcsCharacter)
			return FALSE;

		// Lock 해준다. 2006.05.04. steeple
		if (!pcsCharacter->m_Mutex.WLock())
		{
			m_pcsAgpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
			return NULL;
		}

		lSpawnCharacterCID = pcsCharacter->m_lID;
		pcsCharacter->m_stPos = stPos;


		AgpdSpawnADChar* pcsSpawnADChar = m_pcsAgpmEventSpawn->GetCharacterData(pcsCharacter);
		if( pcsSpawnADChar )
		{
			pcsSpawnADChar->m_pstEvent = pstEvent;
			//pcsSpawnADChar->m_pstSpawn = pstSpawn;
			pcsSpawnADChar->m_pstSpawn = pstSpawnPrime;
		}

		if (pstEvent->m_pcsSource && pstEvent->m_pcsSource->m_eType == APBASE_TYPE_OBJECT)
		{
			ApdObject *	pcsObject = (ApdObject *) pstEvent->m_pcsSource;
			m_pcsAgpmCharacter->TurnCharacter(pcsCharacter, pcsObject->m_fDegreeX, pcsObject->m_fDegreeY);
		}


		// 이벤트 flag 세팅
		SetGiftBoxStatus(pcsCharacter);

		m_pcsAgpmCharacter->UpdateInit(pcsCharacter);

		if (!m_pcsAgsmCharManager->EnterGameWorld(pcsCharacter, FALSE))
		{
			pcsCharacter->m_Mutex.Release();

			m_pcsAgpmCharacter->RemoveCharacter(pcsCharacter->m_lID);

			return NULL;
		}

		if (m_pcsAgsmCharacter)
		{
			AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
			pcsAgsdCharacter->m_bDestroyWhenDie = TRUE;
		}

		if(pstSpawnPrime)
		{
			ApdEventAttachData* pstEvents = &pstSpawnPrime->m_stEvent;

			if(pstEvents)
			{
				if(!m_pcsApmEventManager->InitObject(pcsCharacter, pstEvents))
				{
					ASSERT(!"AgsmEventSpawn::SpawnCharacter() EventManager InitObject() Error !!!");
				}
			}
		}

		VERIFY(EnumCallback(AGSMSPAWN_CB_SPAWN, pcsCharacter, (void *)&lAI2ID));

		pcsCharacter->m_Mutex.Release();
	}

	return TRUE;

	
}

BOOL	AgsmEventSpawn::SetCallbackSpawn(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSPAWN_CB_SPAWN, pfCallback, pClass);
}

BOOL	AgsmEventSpawn::SetCallbackSpawnUsedData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSPAWN_CB_SPAWN_USED_DATA, pfCallback, pClass);
}

BOOL	AgsmEventSpawn::SetCallbackInitReusedCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSPAWN_CB_INIT_REUSED_CHARACTER, pfCallback, pClass);
}

// 2005.07.21. steeple
// From AgpmEventSpawn. 그냥 AGSMSPAWN_CB_SPAWN 을 EnumCallback 해준다.
BOOL AgsmEventSpawn::CBSpawn(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass)
		return FALSE;

	AgsmEventSpawn* pThis = static_cast<AgsmEventSpawn*>(pClass);
	return pThis->EnumCallback(AGSMSPAWN_CB_SPAWN, pData, pCustData);
}

BOOL AgsmEventSpawn::IsActiveSpawn(ApdEvent *pcsEvent)
{
	if (!pcsEvent || !pcsEvent->m_pvData)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("IsActiveSpawn"));

	AuMATRIX	*pstDirection	= NULL;;

	AgpdSpawn	*pcsSpawn	= (AgpdSpawn *) pcsEvent->m_pvData;
	AuPOS		stSpawnPos	= *m_pcsApmEventManager->GetBasePos(pcsEvent->m_pcsSource, &pstDirection);

	AgsmAOICell	*pcsCell	= m_pcsAgsmAOIFilter->m_csAOIMap.GetCell(&stSpawnPos);
	if (!pcsCell)
		return FALSE;

	if (pcsCell->IsAnyActiveCell())
		return TRUE;

	switch (pcsEvent->m_pstCondition->m_pstArea->m_eType) {
	case APDEVENT_AREA_SPHERE:
		{
			if (pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius == 0.0f ||
				pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius > 5000.f)
				return TRUE;

			AuPOS	stAreaPos	= stSpawnPos;

			stAreaPos.x	-= pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius;
			stAreaPos.z -= pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius;

			pcsCell	= m_pcsAgsmAOIFilter->m_csAOIMap.GetCell(&stAreaPos);
			if (!pcsCell || pcsCell->IsAnyActiveCell())
				return TRUE;

			stAreaPos	= stSpawnPos;
			stAreaPos.x -= pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius;
			stAreaPos.z += pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius;

			pcsCell	= m_pcsAgsmAOIFilter->m_csAOIMap.GetCell(&stAreaPos);
			if (!pcsCell || pcsCell->IsAnyActiveCell())
				return TRUE;

			stAreaPos	= stSpawnPos;
			stAreaPos.x += pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius;
			stAreaPos.z -= pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius;

			pcsCell	= m_pcsAgsmAOIFilter->m_csAOIMap.GetCell(&stAreaPos);
			if (!pcsCell || pcsCell->IsAnyActiveCell())
				return TRUE;

			stAreaPos	= stSpawnPos;
			stAreaPos.x += pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius;
			stAreaPos.z += pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius;

			pcsCell	= m_pcsAgsmAOIFilter->m_csAOIMap.GetCell(&stAreaPos);
			if (!pcsCell || pcsCell->IsAnyActiveCell())
				return TRUE;

			return FALSE;
		}
		break;

	default:
		return TRUE;
		break;
	}

	return TRUE;
}

BOOL AgsmEventSpawn::RemoveSpawnCharacter(ApdEvent *pcsEvent)
{
	if (!pcsEvent || !pcsEvent->m_pvData)
		return FALSE;

	AgpdSpawn	*pcsSpawn	= (AgpdSpawn *) pcsEvent->m_pvData;

	for (int i = 0; i < pcsSpawn->m_lTotalCharacter; ++i)
	{
		if(pcsSpawn->m_stSpawnCharacters[i].m_lID == AP_INVALID_CID)
			continue;

		m_pcsAgpmCharacter->RemoveCharacter(pcsSpawn->m_stSpawnCharacters[i].m_lID);
	}

	return TRUE;
}

BOOL AgsmEventSpawn::SetGiftBoxStatus(AgpdCharacter *pcsCharacter)
{
	// no event
	if(m_pcsAgpmDropItem2->GetCurrentEventID() == 0)
		return TRUE;

	if (!pcsCharacter)
		return FALSE;

	if (!m_pcsAgpmCharacter->IsMonster(pcsCharacter))
		return TRUE;

	m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_EVENT_GIFTBOX);

	if (m_pcsAgpmConfig->IsEnableEventItemEffect() == FALSE)
		return TRUE;

	// 특정 확률 계산
	INT32	lRandomNumber	= m_csRandomNumber.randInt(10000);
	INT32	lRate = 30;

	EventMapIter iter = m_pcsAgpmDropItem2->m_mapEventItem.find(m_pcsAgpmDropItem2->GetCurrentEventID());
	if(iter != m_pcsAgpmDropItem2->m_mapEventItem.end())
		lRate = iter->second.m_lRate;

	if (lRandomNumber > lRate)
		return TRUE;

	m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_EVENT_GIFTBOX);

	return TRUE;
}

BOOL AgsmEventSpawn::ReadEventEffectTxt(CHAR* szFileName, BOOL bDecryption)
{
	if(!szFileName || _tcslen(szFileName) == 0)
		return FALSE;

	AuExcelTxtLib csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFileName, TRUE, bDecryption))
		return FALSE;

	INT32 lRow = csExcelTxtLib.GetRow();
	INT32 lCol = csExcelTxtLib.GetColumn();

	INT32 lCurRow = 1;

	while(true)
	{
		INT32 lEventID = csExcelTxtLib.GetDataToInt(0, lCurRow);
		if(lEventID < 1)
		{
			++lCurRow;
			
			if(lCurRow > lRow)
				break;

			continue;
		}

		EventItemEffect stEventItemEffect;
		memset(&stEventItemEffect, 0, sizeof(stEventItemEffect));

		stEventItemEffect.m_lEventID = lEventID;
		stEventItemEffect.m_lEffectID = csExcelTxtLib.GetDataToInt(2, lCurRow);
		stEventItemEffect.m_lRate = csExcelTxtLib.GetDataToInt(3, lCurRow);
		stEventItemEffect.m_lEnable = csExcelTxtLib.GetDataToInt(4, lCurRow);

		if(stEventItemEffect.m_lEnable)
		{
			m_pcsAgpmCharacter->SetEventEffectID(stEventItemEffect.m_lEffectID);
			m_pcsAgpmDropItem2->SetCurrentEventID(stEventItemEffect.m_lEventID);
		}

		while(lCurRow < lRow)
		{
			INT32 lItemTID = csExcelTxtLib.GetDataToInt(1, lCurRow);
			stEventItemEffect.m_vcItemTIDs.push_back(lItemTID);

			++lCurRow;
			if(csExcelTxtLib.GetDataToInt(0, lCurRow) != 0)
				break;
		}

		m_pcsAgpmDropItem2->m_mapEventItem.insert(std::make_pair(lEventID, stEventItemEffect));

		if(lCurRow > lRow)
			break;
	}

	return TRUE;
}

BOOL AgsmEventSpawn::InsertAdditionalSpawn(AgsmSapwnAdditionalType eType, PVOID pClass, ApModuleDefaultCallBack pfCallback, INT32 lData1, INT32 lData2)
{
	if(!pClass || !pfCallback)
		return FALSE;

	m_IdleMutex.WLock();

	AdditionalSpawnData stSpawnData;
	stSpawnData.m_eType = eType;
	stSpawnData.m_pClass = pClass;
	stSpawnData.m_pfCallback = pfCallback;
	stSpawnData.m_lData1 = lData1;
	stSpawnData.m_lData2 = lData2;

	m_vcAdditionalSpawn.push_back(stSpawnData);

	m_IdleMutex.Release();
	
	return TRUE;
}
