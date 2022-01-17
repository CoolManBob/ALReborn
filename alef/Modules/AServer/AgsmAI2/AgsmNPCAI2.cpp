#include "AgsmAI2.h"
#include <math.h>

#define AGSMNPCAI2_FIXED_NPC_RANDE		100.0f

AgpdCharacter *AgsmAI2::GetTargetForGuardNPC(AgpdCharacter *pcsAgpdNPCCharacter, AuPOS csNPCPos, AgpdAI2ADChar *pcsAgpdAI2ADChar )
{
	PROFILE("AgsmAI2::GetTargetPC");

	AgpdAI2ADChar			*pcsTempAI2ADChar;

	INT_PTR				alCID[AGSMAI2_MAX_PROCESS_TARGET*2];
	INT32				lCharListCount;
	INT32				lNumCID;
	//INT32				lCounter;
	INT32				lMaxDamage;
	float				fVisibility;

	lMaxDamage = 0;
	fVisibility = 2000.0f;

	AgpdCharacter	*pcsTargetCharacter	= NULL;

	//주변에 MOB,PC가 있나본다.
	lCharListCount = m_pcsApmMap->GetCharList(pcsAgpdNPCCharacter->m_nDimension, APMMAP_CHAR_TYPE_PC, csNPCPos, fVisibility, alCID, AGSMAI2_MAX_PROCESS_TARGET);
	lNumCID = m_pcsApmMap->GetCharList( pcsAgpdNPCCharacter->m_nDimension, APMMAP_CHAR_TYPE_MONSTER, csNPCPos, fVisibility, &alCID[lCharListCount], AGSMAI2_MAX_PROCESS_TARGET);

	lNumCID += lCharListCount;

	//나(가드) 말고 누군가 있는경우.
	if( lNumCID > 0 )
	{
		AgpdCharacter	*pcsAgroTargetCharacter = NULL;

		float			fAgroDistance = 0.0f;
		INT32			lDamage;

		for (int i = 0; i < lNumCID; ++i)
		{
			AgpdCharacter	*pcsListCharacter	= (AgpdCharacter *) (alCID[i]);

			if(!pcsListCharacter)
				continue;

			//NPC가드 자신은 뺀다.
			if( pcsListCharacter->m_lID == pcsAgpdNPCCharacter->m_lID )
				continue;

			//단~ 같은 유니온의 PC는 제외한다.
			if( m_pcsAgpmCharacter->IsPC( pcsListCharacter ) )
			{
				if (AGPMCHAR_MURDERER_LEVEL1_POINT > m_pcsAgpmCharacter->GetMurdererLevel(pcsListCharacter))
					continue;
			}

			//GM의 경우는 제외한다.
			if( m_pcsAgpmCharacter->IsGM( pcsListCharacter ) )
				continue;

			//죽은 경우 제외한다.
			if( pcsListCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD )
				continue;

			//자리를 지키는 가드 NPC의 경우.
			if(	pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_eNPCAI2Type == AGPMAI2_TYPE_FIXED_NPC )
			{
				float				fTempX, fTempZ, fTempDistance;

				fTempDistance = pcsAgpdAI2ADChar->m_fSpawnRadius;
				//fTempX = pcsListCharacter->m_stPos.x - pcsAgpdNPCCharacter->m_stPos.x;
				//fTempZ = pcsListCharacter->m_stPos.z - pcsAgpdNPCCharacter->m_stPos.z;
				fTempX = pcsListCharacter->m_stPos.x - pcsAgpdAI2ADChar->m_csSpawnBasePos.x;
				fTempZ = pcsListCharacter->m_stPos.z - pcsAgpdAI2ADChar->m_csSpawnBasePos.z;

				//만약 스폰영역 밖에 있는 적이라면 무시한다.
				if( fTempX*fTempX + fTempZ*fTempZ > fTempDistance*fTempDistance )
				{
					continue;
				}
			}

			pcsTempAI2ADChar = m_pcsAgpmAI2->GetCharacterData( pcsListCharacter );

			if (pcsListCharacter->m_Mutex.WLock())
			{
				//나를 때린적이 있는가?
				lDamage = m_pcsAgsmCharacter->GetAgroFromEntry( pcsAgpdNPCCharacter, pcsListCharacter->m_lID );

				pcsListCharacter->m_Mutex.Release();

				if(	lDamage != 0 )
				{
					//리스트에 넣는다.
					if( lDamage < lMaxDamage )
					{
						pcsTargetCharacter = pcsListCharacter;
						lMaxDamage = lDamage;
					}
				}
				//여기까지 왔으면 적대리스트에 추가한다.
				else
				{
					//NPC보다 적의 레벨에 낮으면 선공한다.(어차피 NPC레벨은 높은거라 상관없겠지?^^)
	//				if( m_pcsAgpmFactors->GetLevel( &pcsAgpdNPCCharacter->m_csFactor ) >= m_pcsAgpmFactors->GetLevel( &pcsListCharacter->m_csFactor ) )
					//무조건 선공한다.
					{
						float			fTempX, fTempZ;

						fTempX = pcsListCharacter->m_stPos.x - pcsAgpdNPCCharacter->m_stPos.x;
						fTempZ = pcsListCharacter->m_stPos.z - pcsAgpdNPCCharacter->m_stPos.z;

						//아직 아무도 타켓으로 정해지지 않은 경우.
						if( pcsAgroTargetCharacter == NULL )
						{
							pcsAgroTargetCharacter = pcsListCharacter;
							fAgroDistance = fTempX*fTempX + fTempZ*fTempZ;
						}
						//누군가 이미 타켓으로 정해진 경우.
						else
						{
							if( fAgroDistance > fTempX*fTempX + fTempZ*fTempZ )
							{
								pcsAgroTargetCharacter = pcsListCharacter;
								fAgroDistance = fTempX*fTempX + fTempZ*fTempZ;
							}
						}
					}
				}
			}
		}

		if( (pcsTargetCharacter == NULL) && (pcsAgroTargetCharacter != NULL) )
		{
			pcsTargetCharacter = pcsAgroTargetCharacter;
		}
	}

	//가장 적대치가 높은PC의 ID를 리턴한다!
	return pcsTargetCharacter;
}

BOOL AgsmAI2::ProcessGuardNPCAI( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount )
{
	PROFILE("AgsmAI2::ProcessAI");

	AgpdCharacter	*pcsAgpdNPCCharacter;
	AgpdCharacter	*pcsAgpdTargetCharacter;
	AuPOS			csNPCPos;

	BOOL			bResult;
	INT32			lNPCID, lAggressivePoint;
	float			fTempX, fTempZ, fTempDistance;

	pcsAgpdNPCCharacter = NULL;
	pcsAgpdTargetCharacter = NULL;
	bResult = FALSE;

	if( !pcsAgpdAI2ADChar || !pcsAgpdAI2ADChar->m_pcsAgpdAI2Template )
		return FALSE;

	pcsAgpdNPCCharacter	= pcsAgpdCharacter;

	sprintf( pcsAgpdCharacter->m_szSkillInit, "NPCAI" );

	// 스턴 상태인경우는 암짓도 하면 안된다. 흐흐...
	// 2004.08.25. steeple
	//if (pcsAgpdNPCCharacter->m_unActionStatus == AGPDCHAR_STATUS_STUN)
	if (pcsAgpdNPCCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
		return TRUE;

	csNPCPos = pcsAgpdCharacter->m_stPos;
	lNPCID = pcsAgpdCharacter->m_lID;
	lAggressivePoint = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_lAgressivePoint;

	pcsAgpdTargetCharacter = GetTargetForGuardNPC(pcsAgpdCharacter, csNPCPos, pcsAgpdAI2ADChar );

	//타켓이 있을때.
	if( pcsAgpdTargetCharacter )
	{
		// 소환수는 공격하지 않는다.
		if (m_pcsAgpmCharacter->IsStatusSummoner(pcsAgpdTargetCharacter))
			return TRUE;

		// 크리쳐는 공격하지 않는다.
		if (m_pcsAgpmCharacter->IsCreature(pcsAgpdTargetCharacter))
			return TRUE;

		//시야에 타켓이 있으니 당연히 공격한다. ^^;
		PROFILE("AgsmAI2::AttackTarget");

		BOOL				bInRange;
		BOOL				bPathFind;

		bInRange =  FALSE;
		bPathFind = FALSE;

		bInRange = m_pcsAgpmFactors->IsInRange(&pcsAgpdNPCCharacter->m_stPos, &pcsAgpdTargetCharacter->m_stPos, &pcsAgpdNPCCharacter->m_csFactor, &pcsAgpdTargetCharacter->m_csFactor);

		//공격 범위 안인가?
		if( bInRange )
		{
			//공격해도 되면 공격한다.
			if( lClockCount > pcsAgpdAI2ADChar->m_ulLastAttackTime + pcsAgpdAI2ADChar->m_lAttackInterval )
			{
				if (pcsAgpdTargetCharacter->m_Mutex.WLock())
				{
					m_pcsAgpmCharacter->ActionCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, pcsAgpdNPCCharacter, (ApBase *)pcsAgpdTargetCharacter, NULL);
					pcsAgpdTargetCharacter->m_Mutex.Release();
				}

				pcsAgpdAI2ADChar->m_ulLastAttackTime = lClockCount;
			}
			else
			{
				//만약 이동중이면 멈춘다.
				if (pcsAgpdNPCCharacter->m_bMove)
					m_pcsAgpmCharacter->StopCharacter(pcsAgpdNPCCharacter, NULL);
			}
		}
		//공격범위 밖인가?
		else
		{
			bPathFind = TRUE;
		}

		if( bPathFind )
		{
			float				fTempX, fTempZ;

			fTempX = pcsAgpdTargetCharacter->m_stPos.x - pcsAgpdNPCCharacter->m_stPos.x;
			fTempZ = pcsAgpdTargetCharacter->m_stPos.z - pcsAgpdNPCCharacter->m_stPos.z;

			//fDistance = 500.0f;

			//if( fTempX*fTempX + fTempZ*fTempZ <= fDistance*fDistance )
			{
				pcsAgpdAI2ADChar->m_csTargetPos.x = pcsAgpdTargetCharacter->m_stPos.x;
				pcsAgpdAI2ADChar->m_csTargetPos.z = pcsAgpdTargetCharacter->m_stPos.z;

				//공격전에 길을 찾아둔다.
				bResult = m_pcsAgpmPathFind->pathFind(  pcsAgpdCharacter->m_stPos.x,
														pcsAgpdCharacter->m_stPos.z,
														pcsAgpdAI2ADChar->m_csTargetPos.x,
														pcsAgpdAI2ADChar->m_csTargetPos.z,
														pcsAgpdAI2ADChar->m_pclPath );

				//
				CPriorityListNode< int , AgpdPathFindPoint *> *pcsNode;

				pcsNode = pcsAgpdAI2ADChar->m_pclPath->GetNextPath();

				if( pcsNode )
				{
					AuPOS			csPos;

					csPos.x = pcsNode->data->m_fX;
					csPos.y = 0.0f;
					csPos.z = pcsNode->data->m_fY;

					m_pcsAgpmCharacter->MoveCharacter( pcsAgpdCharacter, &csPos, MD_NODIRECTION, FALSE, TRUE );
				}
			}
		}
	}
	//타켓이 없을때?
	else
	{
		if( lClockCount < pcsAgpdAI2ADChar->m_ulNextWanderingTime )
		{
			//아무일없음.
		}
		else
		{
			//이동중이 아닌데 다음 패스 리스트가 없으면 길을 찾고 
			if( !pcsAgpdCharacter->m_bMove )
			{
				CPriorityListNode< int , AgpdPathFindPoint *> *pcsNode;

				pcsNode = pcsAgpdAI2ADChar->m_pclPath->GetNextPath();

				if( pcsNode )
				{
					AuPOS			csPos;

					csPos.x = pcsNode->data->m_fX;
					csPos.y = 0.0f;
					csPos.z = pcsNode->data->m_fY;

					//NPC가드의 경우는 무조건 뛴다. 현재 걷는애니가 없다. -_-;
					m_pcsAgpmCharacter->MoveCharacter( pcsAgpdNPCCharacter, &csPos, MD_NODIRECTION, FALSE, TRUE );
				}
				//길을 찾고 NextWanderingTime을 리셋한다.
				else
				{
					//우선 고정형 가드 NPC인지, 패트롤형인지 본다.
					if( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_eNPCAI2Type == AGPMAI2_TYPE_FIXED_NPC )
					{
						fTempX = pcsAgpdAI2ADChar->m_csSpawnBasePos.x - pcsAgpdNPCCharacter->m_stPos.x;
						fTempZ = pcsAgpdAI2ADChar->m_csSpawnBasePos.z - pcsAgpdNPCCharacter->m_stPos.z;
						fTempDistance = AGSMNPCAI2_FIXED_NPC_RANDE;

						//원래 자리에서 1미터이상 벗어나있다면 원래 자리로 돌아간다.
						if( fTempX*fTempX + fTempZ*fTempZ > fTempDistance*fTempDistance )
						{
							//타켓을 원래 스폰포인트로 지정해준다.
							pcsAgpdAI2ADChar->m_csTargetPos = pcsAgpdAI2ADChar->m_csSpawnBasePos;

							//바로 원래자리로 돌아간다.
							pcsAgpdAI2ADChar->m_ulNextWanderingTime = lClockCount;

							bResult = m_pcsAgpmPathFind->pathFind(  pcsAgpdNPCCharacter->m_stPos.x,
																	pcsAgpdNPCCharacter->m_stPos.z,
																	pcsAgpdAI2ADChar->m_csTargetPos.x,
																	pcsAgpdAI2ADChar->m_csTargetPos.z,
																	pcsAgpdAI2ADChar->m_pclPath );
						}
						//아니라면 특별히 수행할 일은 없다.
						else
						{
							;//할 일 없음.
						}
					}
					else //if( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_eNPCAI2Type == AGPMAI2_TYPE_PATROL_NPC )
					{
						PROFILE("AgsmAI2::GetNewTargetPosition");

						fTempDistance = pcsAgpdAI2ADChar->m_fSpawnRadius;
						fTempX = pcsAgpdNPCCharacter->m_stPos.x - pcsAgpdAI2ADChar->m_csSpawnBasePos.x;
						fTempZ = pcsAgpdNPCCharacter->m_stPos.z - pcsAgpdAI2ADChar->m_csSpawnBasePos.z;

						//만약 스폰범위안에 존재하지 않는다면 지금 위치를 새로운 스폰포인트로 잡는다.
						if( fTempX*fTempX + fTempZ*fTempZ > fTempDistance*fTempDistance )
						{
							pcsAgpdAI2ADChar->m_csSpawnBasePos = pcsAgpdCharacter->m_stPos;
						}

						if( GetTargetPosFromSpawn(pcsAgpdNPCCharacter, &pcsAgpdNPCCharacter->m_stPos, &pcsAgpdAI2ADChar->m_csTargetPos, &pcsAgpdAI2ADChar->m_csSpawnBasePos, pcsAgpdAI2ADChar->m_fSpawnRadius ) )
						{
							bool			bResult;
							INT32			lRandTime;

							bResult = false;
							lRandTime = m_csRand.randInt()%15000;

							//기본 15초에 0~15초사이의 랜덤한 시간을 더한후 다움에 움직인다.
							pcsAgpdAI2ADChar->m_ulNextWanderingTime = lClockCount + 15000 + lRandTime;

							//새로운 목적지가 세팅되었으니 길을 찾아보세나~
							bResult = m_pcsAgpmPathFind->pathFind(  pcsAgpdNPCCharacter->m_stPos.x,
																	pcsAgpdNPCCharacter->m_stPos.z,
																	pcsAgpdAI2ADChar->m_csTargetPos.x,
																	pcsAgpdAI2ADChar->m_csTargetPos.z,
																	pcsAgpdAI2ADChar->m_pclPath );

							if( bResult )
							{
								//AgpmCharacter::OnIdle()에서 CallBack호출! GetNextNode를 호출하고 이동한다.
							}
							else
							{
								//이건 길이 없다는 이야기인데.... 그냥 무시해야지 뭐.... 쩝.
							}
						}
					}
				}
			}
		}
	}

	return bResult;
}